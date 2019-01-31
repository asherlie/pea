/*
 * ~PEA~
 *
 * Pea
 * Enables
 * Amendment
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h> // umask

#include <signal.h>

#include <syslog.h>

#include "pet.h"

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

struct petition_container* pc;

void signal_handler(int signum, siginfo_t* info, void* extra){
      int packed_int = info->si_value.sival_int;
      int operation = packed_int & 0xffff;
      int pet_num = (packed_int >> 16) & 0xffff;
      FILE* fp = fopen("/home/asher/boybo", "a");
      fprintf(fp, "sig num: %i, received int: %i on pet num %i from user: %i\n", signum,  operation, pet_num, info->si_uid);
      fclose(fp);
      switch(operation){
            // TODO: implement this
            case LIST_PET:
                  break;
            case SIGN_PET:
                  add_signature(pc->petitions[pet_num], info->si_uid);
                  fprintf(fp, "signature from user: %i added to petition %i\n", info->si_uid, pet_num);
                  break;
            case CREATE_PET:
                  insert_p(alloc_p(), pc);
                  fprintf(fp, "new petition created by user: %i\n", info->si_uid);
                  break;
      }
      /*fclose(fp);*/
}

// TODO: if debug_mode, do not fork and print rather than syslog
int pea_daem(_Bool debug_mode){
      pid_t pid, sid;
      if(!debug_mode){
            pid = fork();
            if(pid < 0)exit(EXIT_FAILURE);
            // parent is killed
            if(pid > 0)exit(EXIT_SUCCESS);
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

      struct sigaction action;
      action.sa_flags = SA_SIGINFO;
      action.sa_sigaction = &signal_handler;
      if(sigaction(SIGHUP, &action, NULL) == -1){
            syslog(LOG_ERR, "sigaction failed\n");
            exit(EXIT_FAILURE);
      }
      while(1){
            usleep(1000);
      }
      return 0;
}

int main(){
      pc = malloc(sizeof(struct petition_container));
      init_pc(pc);
      return pea_daem(0);
}
