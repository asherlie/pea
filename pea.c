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

int pea_daem(){
      pid_t pid, sid;
      pid = fork();
      if(pid < 0)exit(EXIT_FAILURE);
      // parent is killed
      if(pid > 0)exit(EXIT_SUCCESS);
      // for linux
      umask(0);
      FILE* logfp = fopen("/var/log/pea_log.log", "a");
      fprintf(logfp, "PET daemon started at pid: %i\n", pid);
      // make new SID for child proc
      sid = setsid();
      if(sid < 0){
            fputs("failed to create new SID\n", logfp);
            fclose(logfp);
            exit(EXIT_FAILURE);
      }
      if((chdir("/")) < 0){
            fputs("failed to cd /\n", logfp);
            fclose(logfp);
            exit(EXIT_FAILURE);
      }
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);

      /* we're now a ~~daemon~~ */

      while(1){
            usleep(1000);
      }
      fclose(logfp);
      return 1;
}

int main(){
      pea_daem();
}
