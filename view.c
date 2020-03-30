#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

int main(void){
    int unread = -1;
    char *buf;
    char c;
    int fd = open("./DB.txt", O_RDONLY);
    // FILE *fp = fopen("./DB.txt", "r");
    if(fd == -1) {
    // if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    // get file size by fd
    // off_t fsize;
    // fsize = lseek(fd, 0, SEEK_END);
    
    // wait for stdin
    // while(unread<1){
    //     if(ioctl(STDIN_FILENO,FIONREAD,&unread)){
    //         perror("ioctl");
    //         exit(EXIT_FAILURE);
    //     }
    // }

    // buf = (char*)malloc(sizeof(char) * (fsize + 1));
    
    // printf("fd = %d; fsize = %d; bufsize = %d\n", fd, fsize, sizeof(*buf));

    // read from stdin fd
    // read(fd,buf,fsize);

    getchar();

    // output to stdout
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/text; charset=UTF-8\r\n\r\n");
    printf("TESTtestTEST\n");
    // printf("<!DOCTYPE html>\r\n");
    // printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    // printf("<TITLE>I'm a example</TITLE>\n");
    // printf("<BODY>%s</BODY></HTML>\n",buf);
    // printf("buf: %s", buf);
    sendfile(STDOUT_FILENO, fd, NULL, 87);

    // while (read(fd, &c, 1) > 0) {
    //     write(STDIN_FILENO, &c, 1);
    // }

    // printf("file: \n");
    // while (fscanf(fp, "%c", c) != EOF) {
    //     printf("%c", c);
    // }
    
}