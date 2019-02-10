#include "../PET_OPT.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

char empty_str_arg[50] = {0};

_Bool strtoi(char* str, int* i){
      char* res;
      unsigned int r = (unsigned int)strtol(str, &res, 10);
      if(*res)return 0;
      *i = (int)r;
      return 1;
}

// packs an integer into the first and last 16 bits of an int
int pack_int(int x, int y){
      return x | (y << 16);
}

// petition option and reference number are packed into a single int before being sent
// ref_num can be zero in the case of CREATE_PET
// pet_label MUST be a reference to a char[50]
_Bool pet_connect(char* sock_path, int pet_opt, int ref_num, char* pet_label){
      int sock = socket(AF_UNIX, SOCK_STREAM, 0);
      if(sock == -1)perror("socket");
      struct sockaddr_un remote;
      memset(&remote, 0, sizeof(struct sockaddr_un));
      remote.sun_family = AF_UNIX;
      strcpy(remote.sun_path, sock_path);
      // this is all that is necessary for pea to receive our credentials
      if(connect(sock, (struct sockaddr*)&remote, sizeof(struct sockaddr_un)) == -1)perror("connect");
      // sending our petition option
      // this integer uses the first 16 bits for pet_opt and second for ref
      // TODO: ref_num should be 29 bits - we only need 3 for pet_opt 2
      // TODO: check size of ref_num
      // if(ref_num > 16 bits) ...
      int snd_val = pack_int(pet_opt, ref_num);
      int ret;
      if((ret = send(sock, &snd_val, sizeof(int), 0) == -1))perror("send");
      if((ret = send(sock, pet_label, 50, 0)) == -1)perror("send");
      close(sock);
      return ret != -1;
}

_Bool sign_petition(char* sock_path, int ref_num){
      return pet_connect(sock_path, SIGN_PET, ref_num, empty_str_arg);
}

// TODO: possibly default to /tmp/pea/pet_sock for socket
// location to make it easier for user
/*
 * usage:
 *   ./pea socket_file list                 // list available petitions
 *   ./pea socket_file sign list_item       // sign petition
 *   ./pea socket_file create petition_name // create new petition
 *
 */

void p_usage(char* a){
      printf("usage:\n  %s [socket_file] \"list\" - lists available petitions\
               \n  %s [socket_file] \"sign\" [list_item] - signs petition\
               \n  %s [socket_file] \"create\" [petition_name] - creates new petition\
               \n  %s [socket_file] \"unsign\" [list_item] - removes signature from petition\
               \n  %s [socket_file] \"import\" [backup_file_path] - creates a petition to import backup file\n", a, a, a, a, a);
}

int main(int argc, char** argv){
      if(argc < 3){
            p_usage(*argv);
            return 1;
      }
      char* s_path = argv[1];
      // list petitions to outfile
      if(*argv[2] == 'l')
            return !pet_connect(s_path, LIST_PET, 0, empty_str_arg);
      // create, sign, unsign, remove need 2 additional args
      if(argc < 4){
            p_usage(*argv);
            return 1;
      }
      // create petition
      if(*argv[2] == 'c')
            return !pet_connect(s_path, CREATE_PET, 0, argv[3]);
      // import petition backup file
      if(*argv[2] == 'i')
            return !pet_connect(s_path, IMPORT_PET, 0, argv[3]);
      int pet_num = -1;
      if(!strtoi(argv[3], &pet_num)){
            p_usage(*argv);
            return 1;
      }
      // sign petition
      if(*argv[2] == 's')
            return !sign_petition(s_path, pet_num);
      // remove petition from pc
      if(*argv[2] == 'r')
            return !pet_connect(s_path, RM_PET, pet_num, empty_str_arg);
      // unsign petition
      if(*argv[2] == 'u')
            return !pet_connect(s_path, RM_SIG, pet_num, empty_str_arg);
      p_usage(*argv);
      return 0;
}
