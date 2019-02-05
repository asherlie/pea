#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

/*strtol();*/
// packs an integer into the first and last 16 bits of an int
int pack_int(int x, int y){
      return x | (y << 16);
}

// petition option and reference number are packed into a single int before being sent
// ref_num can be zero in the case of CREATE_PET
int pet_connect(char* sock_path, int pet_opt, int ref_num){
      int sock = socket(AF_UNIX, SOCK_STREAM, 0);
      struct sockaddr_un remote;
      memset(&remote, 0, sizeof(struct sockaddr_un));
      remote.sun_family = AF_UNIX;
      strcpy(remote.sun_path, sock_path);
      // this is all that is necessary for for pea to receive our credentials
      connect(sock, (struct sockaddr*)&remote, sizeof(struct sockaddr_un));
      // sending our petition option
      // this integer uses the first 16 bits for pet_opt and second for ref
      int snd_val = pack_int(pet_opt, ref_num);
      return (send(sock, &snd_val, sizeof(int), 0) == -1);
}

_Bool sign_petition(char* sock_path, int ref_num){
      return pet_connect(sock_path, SIGN_PET, ref_num) != -1;
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
               \n  %s [socket_file] \"create\" - creates new petition\n", a, a, a);
}

int main(int argc, char** argv){
      if(argc < 3){
            p_usage(*argv);
            return 1;
      }
      char* s_path = argv[1];
      // list petitions to outfile
      if(*argv[2] == 'l'){
            pet_connect(s_path, LIST_PET, 0);
            return 0;
      }
      // create petition
      if(*argv[2] == 'c'){
            pet_connect(s_path, CREATE_PET, 0);
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
