#include <stdio.h>
#include <error.h>
#include <stdlib.h>

int main(void){
    // open file
    FILE *fp = fopen("./DB.txt", "r");
    if (!fp) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    // output to stdout
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/text; charset=UTF-8\r\n\r\n");
    
    // FILE pointer
    int c; // note: int, not char, required to handle EOF
    while ((c = fgetc(fp)) != EOF) { // standard C I/O file reading loop
       putchar(c);
    }
    fclose(fp);
}