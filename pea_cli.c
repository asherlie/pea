#include <signal.h>
#include <stdio.h>

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
      // sign petition
      if(*argv[1] == 's'){
            return 0;
      }
      // create petition
      if(*argv[1] == 'c'){
            return 0;
      }
      return 0;
}
