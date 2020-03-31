#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

int main(void){
    char *buf;
    // int fd = open("./DB.txt", O_RDONLY);
    FILE *fp = fopen("./DB.txt", "r");
    // if(fd == -1) {
    if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }


    getchar();

    // output to stdout
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/text; charset=UTF-8\r\n\r\n");
    printf("printf TEST\n");

    // buf = (char*)malloc(sizeof(char) * (fsize + 1));
    // read from stdin fd
    // read(fd,buf,fsize);
    // printf("buf: %s", buf);
    
    // sendfile
    // sendfile(STDOUT_FILENO, fd, NULL, fsize);

    // fd
    // get file size by fd
    // off_t fsize;
    // fsize = lseek(fd, 0, SEEK_END);
    // printf("fd = %d; fsize = %d; bufsize = %d\n", fd, fsize, sizeof(*buf));
    // while (read(fd, &c, 1) > 0) {
    //     printf("0");
    // }

    // FILE pointer
    int c; // note: int, not char, required to handle EOF
    while ((c = fgetc(fp)) != EOF) { // standard C I/O file reading loop
       putchar(c);
    }
    fclose(fp);
    
}