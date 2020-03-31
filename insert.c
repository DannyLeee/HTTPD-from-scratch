#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>

/*
Please make sure you understand host.c
*/

int main(void){
    int unread = -1;
    char *buf;

    FILE *fp = fopen("./DB.txt", "a");
    if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    // wait for stdin
    while(unread<1){
        if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
            perror("ioctl");
            exit(EXIT_FAILURE);
        }
    }
    buf = (char*)malloc(sizeof(char)*(unread+1));
    
    // read from stdin fd
    read(STDIN_FILENO,buf,unread);
    fputs(buf, fp);



    // output to stdout
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
    printf("<!DOCTYPE html>\r\n");
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>Insert complete</TITLE>\n");
    printf("<BODY><center><h1>Insert complete!!</h1><br>");
    printf("%s<center></BODY></HTML>\n", buf);
}