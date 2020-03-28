#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


#define DEBUG

/*
program.c   ---> CGI program
Host.c      ---> main program

In this example, Host will call the CGI program 
and send the message ("Hello world") to CGI program, 
then the CGI program will return the result to Host.
*/

int main(){

      int cgiInput[2];
      int cgiOutput[2];
      int status;
      char* inputData={"Hello world"};
      pid_t cpid;
      char c;

      /* Use pipe to create a data channel betweeen two process
         'cgiInput'  handle  data from 'host' to 'CGI'
         'cgiOutput' handle data from 'CGI' to 'host'*/
      if(pipe(cgiInput)<0){
            perror("pipe");
            exit(EXIT_FAILURE);
      }
      if(pipe(cgiOutput)<0){
            perror("pipe");
            exit(EXIT_FAILURE);
      }

      /* Creates a new process to execute cgi program */
      if((cpid = fork()) < 0){
            perror("fork");
            exit(EXIT_FAILURE);
      }
      
      /*child process*/
      if(cpid == 0){
            printf("this is child process\n");

            //close unused fd
            close(cgiInput[1]);
            close(cgiOutput[0]);

            //redirect the output from stdout to cgiOutput
            dup2(cgiOutput[1],STDOUT_FILENO);

            //redirect the input from stdin to cgiInput
            dup2(cgiInput[0], STDIN_FILENO); 
            
            //after redirect we don't need the old fd 
            close(cgiInput[0]);
            close(cgiOutput[1]);

            /* execute cgi program
               the stdout of CGI program is redirect to cgiOutput
               the stdin  of CGI program is redirect to cgiInput
            */
            execlp("./program.cgi","./program.cgi",NULL);
            exit(0);
      }

      /*parent process*/
      else{ 
            printf("this is parent process\n");


            //close unused fd
            close(cgiOutput[1]);
            close(cgiInput[0]);
            
            // send the message to the CGI program
            write(cgiInput[1], inputData, strlen(inputData));

            // receive the message from the  CGI program
            while (read(cgiOutput[0], &c, 1) > 0){

                  // output the message to terminal
                  write(STDOUT_FILENO, &c, 1);
            }

            send(STDOUT_FILENO, "\n", 1, 0);


            // connection finish
            close(cgiOutput[0]);
            close(cgiInput[1]);
            waitpid(cpid, &status, 0);
        }
 
}