#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

char empty_str_arg[50] = {0};

/*strtol();*/
// packs an integer into the first and last 16 bits of an int
int pack_int(int x, int y){
      return x | (y << 16);
}

// petition option and reference number are packed into a single int before being sent
// ref_num can be zero in the case of CREATE_PET
// pet_label MUST be a reference to a char[50]
int pet_connect(char* sock_path, int pet_opt, int ref_num, char* pet_label){
      int sock = socket(AF_UNIX, SOCK_STREAM, 0);
      struct sockaddr_un remote;
      memset(&remote, 0, sizeof(struct sockaddr_un));
      remote.sun_family = AF_UNIX;
      strcpy(remote.sun_path, sock_path);
      // this is all that is necessary for for pea to receive our credentials
      if(connect(sock, (struct sockaddr*)&remote, sizeof(struct sockaddr_un)) == -1)perror("connect");
      // sending our petition option
      // this integer uses the first 16 bits for pet_opt and second for ref
      int snd_val = pack_int(pet_opt, ref_num);
      int ret;
      if((ret = send(sock, &snd_val, sizeof(int), 0) == -1))perror("send");
      if((ret = send(sock, pet_label, 50, 0)) == -1)perror("send");
      return ret;
}

_Bool sign_petition(char* sock_path, int ref_num){
      return pet_connect(sock_path, SIGN_PET, ref_num, empty_str_arg) != -1;
}

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
               \n  %s [socket_file] \"create\" [petition_name] - creates new petition\n", a, a, a);
}

int main(int argc, char** argv){
      if(argc < 3){
            p_usage(*argv);
            return 1;
      }
      char* s_path = argv[1];
      // list petitions to outfile
      if(*argv[2] == 'l'){
            pet_connect(s_path, LIST_PET, 0, empty_str_arg);
            return 0;
      }
      // create petition
      if(*argv[2] == 'c'){
            if(argc < 4){
                  p_usage(*argv);
                  return 1;
            }
            pet_connect(s_path, CREATE_PET, 0, argv[3]);
            return 0;
      }
      // sign requires 2 args
      if(argc < 4){
            p_usage(*argv);
            return 1;
      }
      // TODO: use my strtoi and make this safer
      // bound checking, etc.
      // sign petition
      if(*argv[2] == 's'){
            int pet_num = atoi(argv[3]);
            sign_petition(s_path, pet_num);
            return 0;
      }
      p_usage(*argv);
      return 0;
}
