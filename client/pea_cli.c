#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

int send_sig(pid_t pid, int pet_opt){
      union sigval val;
      val.sival_int = pet_opt;
      return sigqueue(pid, SIGHUP, val) == 0;
}

_Bool sign_petition(pid_t pid){
      return send_sig(pid, SIGN_PET);
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
      // list
      if(*argv[2] == 'l'){
            return 0;
      }
      // sign and create both require 2 args
      if(argc < 4)return 1;
      // sign petition
      if(*argv[2] == 's'){
            // TODO: use my strtoi and make this safer
            // bound checking, etc.
            pid_t pid = atoi(argv[1]); 
            sign_petition(pid);
            return 0;
      }
      // create petition
      if(*argv[1] == 'c'){
            return 0;
      }
      return 0;
}
