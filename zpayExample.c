#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


char webPage[] = 
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>webServer</title></head>\r\n"
"<body><center><h3>Welcome to the server</h3><br>\r\n"
"<form enctype='multipart/form-data' action='.' method='POST'>"
"<input name='text' type='text' /><br>"
"<input type='submit' />"
"</form>"
"</center></body></html>\r\n";
/*The apperence of web server when browser connnect*/

char imageheader[] = 
"HTTP/1.1 200 Ok\r\n"
"Content-Type: image/jpeg\r\n\r\n";
/*The header that loading image*/

int main(int argc, char *argv[]){

	struct sockaddr_in server_addr, client_addr; // the address of the socket(internet format)
	socklen_t sin_len = sizeof(client_addr);//length of cnt socket	// for the portability
	int fd_server , fd_client;//file descriptor //the return value of the soket function
	char bfr[2048];	// storing the returned or the content sent by the browser to the web server
	int fdimg;//holding fd of file opened
	int on = 1;
	
	fd_server = socket(AF_INET, SOCK_STREAM,0);	// open a socket (AF_INET: Internet address. \
	SOCK_STREAM: Provides sequenced, reliable, bidirectional, connection-mode byte streams, \
	and may provide a transmission mechanism for out-of-band data.)
	if (fd_server < 0){
		perror("socket");
		exit(1);
	}
	/*means something went wrong*/
	
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));	//  set the socket property \
	SOL_SOCKET: set options at the socket level\
	SO_REUSEADDR: supplied to bind() should allow reuse of local addresses

	server_addr.sin_family = AF_INET;	// Address family
	server_addr.sin_addr.s_addr = INADDR_ANY;	// Internet address
	server_addr.sin_port = htons(8080);	// port num
	
	if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1){	// assigns an address to an unnamed socket \
	2nd the address bind to the socket
		perror("bind");
		close(fd_server);
		exit(1);
	}

	if (listen(fd_server,10) == -1){	// listen for socket connections and limit the queue of incoming connections
		perror("Listen");
		close(fd_server);
		exit(1);
	}
	
	while(1){
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);	// accept a new connection on a socket \
		做一個跟1st一樣的socket
		
		if (fd_client == -1){
			perror("Connection failed\n");
			continue;
		}
		
		printf("--------Got client connection--------\n");

		if (!fork()){ //child process
			close(fd_server);	// child doesn't need
			memset(bfr,0,2048);	// reset buf
			read(fd_client, bfr, 2047);	// read 2047 bytes from fd to buf
			printf("%s\n", bfr);	// print request to terminal

			if (!strncmp(bfr,"GET /test.jpg",13)){	// find get with test.jpg
				write(fd_client, imageheader, sizeof(imageheader) - 1);
				fdimg = open("test.jpg",O_RDONLY);
				sendfile(fd_client,fdimg,NULL,21000);	// transfer data between file descriptors
				close(fdimg);
			}
			else if (!strncmp(bfr,"POST /",5)){
				write(fd_client, imageheader, sizeof(imageheader) - 1);
				fdimg = open("test.jpg",O_RDONLY);
				sendfile(fd_client,fdimg,NULL,21000);
				close(fdimg);
				//printf("Get file!\n");
			}
			else{
				write(fd_client,webPage, sizeof (webPage) - 1);
			}
			close(fd_client);
			printf("closing\n");
			exit(0);
		}
		//parent process

		close(fd_client);
	}


	return 0;
}