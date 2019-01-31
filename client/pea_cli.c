#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

// packs an integer into the first and last 16 bits of an int
int pack_int(int x, int y){
      return x | (y << 16);
}

// petition option and reference number are packed into a single int before being sent
// ref_num can be zero in the case of CREATE_PET
int send_sig(pid_t pid, int pet_opt, int ref_num){
      union sigval val;
      val.sival_int = pack_int(pet_opt, ref_num);
      return sigqueue(pid, SIGHUP, val) == 0;
}

_Bool sign_petition(pid_t pid, int pet_num){
      /*return send_sig(pid, pack_int(SIGN_PET, pet_num));*/
      return send_sig(pid, SIGN_PET, pet_num);
}

/*
 * usage:
 *   ./pea pid list // list available petitions
 *   ./pea pid sign list_item // sign petition
 *   ./pea pid create petition_name // create new petition
 *
 */

int main(int argc, char** argv){
      if(argc < 3)return 1;
      pid_t pid = atoi(argv[1]);
      // list petitions to outfile
      if(*argv[2] == 'l'){
            send_sig(pid, LIST_PET, 0);
            return 0;
      }
      // create petition
      if(*argv[2] == 'c'){
            send_sig(pid, CREATE_PET, 0);
            return 0;
      }
      // sign requires 2 args
      if(argc < 4)return 1;
      // TODO: use my strtoi and make this safer
      // bound checking, etc.
      // sign petition
      if(*argv[2] == 's'){
            int pet_num = atoi(argv[3]);
            sign_petition(pid, pet_num);
            return 0;
      }
      return 0;
}
