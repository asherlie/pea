/*
 *    @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@
 *    @            =====                @
 *    @            |PEA|                @
 *    @            =====                @
 *    @  Pea                 Pea        @
 *    @  Enables     OR      Expresses  @
 *    @  Amendment           Annoyance  @
 *    @                                 @
 *    @                                 @
 *    *%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@
 *
 *
 */

#define _GNU_SOURCE

#include "../PET_OPT.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h> // umask

#include <sys/socket.h>
#include <sys/un.h>

#include <syslog.h>

#include "backup.h"

char* pet_f_pth = NULL;
struct petition_container* pc;

uid_t get_peer_cred(int p_sock){
      uid_t uid;
      #ifdef SO_PEERCRED
      socklen_t len = sizeof(struct ucred);
      struct ucred cred;
      memset(&cred, 0, sizeof(struct ucred));
      getsockopt(p_sock, SOL_SOCKET, SO_PEERCRED, &cred, &len);
      uid = cred.uid;
      #else
      gid_t gid;
      getpeereid(p_sock, &uid, &gid);
      #endif
      return uid;
}

// TODO: pet_handler should return meaningful errno
int pet_handler(int p_sock, int packed_int, char* str_arg){
      uid_t p_uid = get_peer_cred(p_sock);
      // splitting our packed int
      int operation = packed_int & 0xffff;
      int pet_num = (packed_int >> 16) & 0xffff;
      
      // for petition writing
      FILE* fp = NULL;
      _Bool update_pf = 0;
      switch(operation){
            case PRINT_PET:
                  fp = fopen(str_arg, "w");
                  if(!fp)break;
                  print_sigs(fp, pc);
                  fclose(fp);
                  break;
            case SIGN_PET:
                  if(pet_num < 0 || pc->n <= pet_num)break;
                  // TODO: abstract this to a function in backup.c
                  // add_signature will return 1 if we can import pc->petitions[pet_num]->restore
                  // update_pf will be set to whether or not petition has been inserted successfully
                  if(add_signature(pc->petitions[pet_num], p_uid, &update_pf)){
                        merge_pet(pc, pc->petitions[pet_num]->restore);
                        for(; pc->petitions[pet_num]->restore->n; remove_p(pc->petitions[pet_num]->restore, 0));
                        free(pc->petitions[pet_num]->restore->petitions);
                        free(pc->petitions[pet_num]->restore);
                        pc->petitions[pet_num]->restore = NULL;
                  }
                  break;
            case CREATE_PET:
                  update_pf = 1;
                  insert_p(alloc_p(), pc, p_uid, str_arg);
                  break;
            case RM_PET:
                  // checking for out of bounds issues and credentials
                  // only she who created a petition can remove it
                  if(pet_num < 0 || pc->n <= pet_num || pc->petitions[pet_num]->creator != p_uid)break;
                  update_pf = 1;
                  remove_p(pc, pet_num);
                  break;
            case RM_SIG:
                  if(pet_num < 0 || pc->n <= pet_num)break;
                  update_pf = remove_sig(pc->petitions[pet_num], p_uid);
                  break;
            case IMPORT_PET:
                  update_pf = 1;
                  // TODO: add error handling for bad filepath
                  // once this petition is signed by all creators in the 
                  setup_import_pet(pc, str_arg);
                  break;
      }
      if(update_pf){
            fp = (DEBUG) ? stdout : fopen(pet_f_pth, "w");
            print_sigs(fp, pc);
            if(!DEBUG)fclose(fp);
      }
      return 0;
}

// TODO: if debug_mode, do not fork and print rather than syslog
int pea_daem(int local_sock, _Bool debug_mode){
      pid_t pid, sid;
      if(!debug_mode){
            pid = fork();
            if(pid < 0)exit(EXIT_FAILURE);
            // parent is killed
            if(pid > 0){
                  printf("daemon spawned at pid: %i\n", pid);
                  exit(EXIT_SUCCESS);
            }
            umask(0);
            openlog("pea", LOG_PID, LOG_USER);
            syslog(LOG_INFO, "PEA daemon started at pid: %i\n", pid);
            // make new SID for child proc
            sid = setsid();
            if(sid < 0){
                  syslog(LOG_ERR, "failed to create new SID\n") ;
                  exit(EXIT_FAILURE);
            }
            if((chdir("/")) < 0){
                  syslog(LOG_ERR, "failed to chdir(\"/\")\n");
                  exit(EXIT_FAILURE);
            }
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
      }

      /* we're now a ~~daemon~~ */

      // disable blocking - acts as a timeout in case not all information is sent
      // this might not do anything - try with peer_sock below
      // commented out bc of cpu usage
      /*
       *int flags = fcntl(local_sock, F_GETFL, 0);
       *fcntl(local_sock, F_SETFL, flags | O_NONBLOCK);
       */

      int peer_sock = -1;
      char pet_label[50] = {0};
      while(1){
            // TODO: destroy socket and reopen before each iteration of this while loop
            // to ensure that nobody stays connected
            int packed_int = -1;
            peer_sock = accept(local_sock, NULL, NULL);
            read(peer_sock, &packed_int, sizeof(int));
            read(peer_sock, pet_label, 50);
            // TODO: return errnos to client
            pet_handler(peer_sock, packed_int, pet_label);
            peer_sock = -1;
      }
      return 0;
}

int sock_init(char* path){
      // if this socket already exists, remove it
      remove(path);
      struct sockaddr_un addr;
      memset(&addr, 0, sizeof(struct sockaddr_un));
      addr.sun_family = AF_UNIX; 
      strcpy(addr.sun_path, path);

      int sock = socket(AF_UNIX, SOCK_STREAM, 0);
      if(sock == -1)perror("socket");
      if(bind(sock, (struct sockaddr*)&addr, SUN_LEN(&addr)) == -1)perror("bind");
      chmod(addr.sun_path, 0777);
      if(listen(sock, 0) == -1)perror("listen");
      return sock;
}

/*
 * argv[1] contains the path to a socket location
 * argv[2] contains a petition output file
 */
int main(int argc, char** argv){
      if(argc < 3){
            printf("usage: %s [socket file] [petition output file]\n", *argv);
            return 1;
      }
      pet_f_pth = argv[2];
      if(*argv[1] != '/' || *pet_f_pth != '/'){
            puts("socket file and petition output file must be complete filepaths");
            return 1;
      }
      // why open a useless file in case of DEBUG
      // TODO: unless a full path is provided this is not a strict enough check
      // this is due to chdir("/") in pea_daem
      // TODO: convert this to a complete filepath if it's not one already
      FILE* tst_fp = fopen(pet_f_pth, "w");
      if(!tst_fp){
            puts("enter a valid file path");
            return 1;
      }
      /* setting read only permissions for all but this user
       * this is not ideal because the user that spawned the daemon
       * can edit the petition file
       * as long as all have read access and can send LIST_PET
       * shouldn't be a problem
       */
      chmod(pet_f_pth, 0644);
      fclose(tst_fp);
      pc = malloc(sizeof(struct petition_container));
      init_pc(pc);
      int l_sock = sock_init(argv[1]);
      return pea_daem(l_sock, DEBUG);
}
