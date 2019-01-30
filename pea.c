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

#define LIST_PET   0
#define SIGN_PET   1
#define CREATE_PET 2

// borrowed from github
void signal_handler(int signum, siginfo_t* info, void* extra){
      int operation = info->si_value.sival_int;
      FILE* fp = fopen("/home/asher/boybo", "a");
      fprintf(fp, "sig num: %i, received int: %i\n", signum,  operation);
      fclose(fp);
      /*return (operation > 2 || operation < 0) operation;*/
}

int pea_daem(){
      pid_t pid, sid;
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

      /* we're now a ~~daemon~~ */

      struct sigaction action;
      action.sa_flags = SA_SIGINFO;
      action.sa_sigaction = &signal_handler;
      /*accept();*/
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
      return pea_daem();
}
