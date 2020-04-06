#include <stdio.h>
#include <error.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    // open file
    FILE *fp = fopen("./DB.txt", "a");
    if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    // write to file
    fputs(argv[1], fp);
    fputs("\n", fp);

    // output to stdout
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=UTF-8\r\n\r\n");
    printf("<!DOCTYPE html>\r\n");
    printf("<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n");
    printf("<TITLE>Insert complete!!</TITLE>\n");
    printf("<BODY><center><b>Inserted</b> %s", argv[1]);
    printf("<center></BODY></HTML>\n");  
}