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
      /*val.sival_ptr = NULL;*/
      return sigqueue(pid, SIGHUP, val) == 0;
}

_Bool sign_petition(pid_t pid){
      /*return send_sig(pid, SIGN_PET);*/
      return send_sig(pid, 69);
}

/*
 * usage:
 *   ./pea list // list available petitions
 *   ./pea sign list_item // sign petition
 *   ./pea create petition_name // create new petition
 *
 */

int main(int argc, char** argv){
      if(argc < 2)return 1;
      // list
      if(*argv[1] == 'l'){
            return 0;
      }
      // sign and create both require 2 args
      if(argc < 3)return 1;
      char uname[10];
      uname[9] = 0;
      getlogin_r(uname, 9);
      // sign petition
      if(*argv[1] == 's'){
            // TODO: use my strtoi and make this safer
            // bound checking, etc.
            pid_t pid = atoi(argv[2]); 
            sign_petition(pid);
            return 0;
      }
      // create petition
      if(*argv[1] == 'c'){
            return 0;
      }
      return 0;
}
