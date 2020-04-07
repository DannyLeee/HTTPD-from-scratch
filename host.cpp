#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

// #define DEBUG

char webPage[] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>webServer</title></head>\r\n"
"<body><center><h3>Welcome to the 8787 server</h3><br>\r\n"
"<a href='./view.cgi'>view</a>"
"<form action='insert.cgi' method='GET'>"
"<input name='data' type='text' />"
"<input type='submit' value='GET'/>"
"<input type='submit' formmethod='post' value='POST'/>"
"</form>"
"</center></body></html>\r\n";

int main(){
	int cgiOutput[2];
	int status;
	char* inputData;
	pid_t cpid;
	char c;

	/*socket initial*/
	struct sockaddr_in server_addr, client_addr;    // the address of the socket(internet format)
	socklen_t sin_len = sizeof(client_addr);  // length of cnt socket	// for the portability
	int fd_server , fd_client;    // file descriptor // the return value of the soket function
	char bfr[2048];	// storing the the content sent by the browser to server
	int on = 1;

	// open a socket (AF_INET: Internet address. \
	SOCK_STREAM: Provides sequenced, reliable, bidirectional, connection-mode byte streams, \
	and may provide a transmission mechanism for out-of-band data.)
	fd_server = socket(AF_INET, SOCK_STREAM,0);
	if (fd_server < 0) {
		perror("socket");
		exit(1);
	}

	//  set the socket property \
	SOL_SOCKET: set options at the socket level\
	SO_REUSEADDR: supplied to bind() should allow reuse of local addresses
	setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	server_addr.sin_family = AF_INET;	// Address family
	server_addr.sin_addr.s_addr = INADDR_ANY;	// Internet address
	server_addr.sin_port = htons(8787);	// port num

	// assigns an address to an unnamed socket \
	2nd the address bind to the socket
	if (bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
		perror("bind");
		close(fd_server);
		exit(1);
	}

	// listen for socket connections and limit the queue of incoming connections
	if (listen(fd_server,10) == -1) {
		perror("Listen");
		close(fd_server);
		exit(1);
	}


	while(1) {
		/* Use pipe to create a data channel betweeen two process
		'cgiOutput' handle data from 'CGI' to 'host'*/
		if(pipe(cgiOutput)<0) {
			perror("pipe");
			exit(EXIT_FAILURE);
		}

		// accept a new connection on a socket \
		做一個跟1st一樣的socket
		fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);

		if (fd_client == -1) {
			perror("Connection failed\n");
			continue;
		}

		printf("--------- Got client connection ---------\n");

		/* Creates a new process to execute cgi program */
		if((cpid = fork()) < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		/*child process*/
		if(cpid == 0) {
#ifdef DEBUG
			printf("this is child process\n");
#endif
			close(fd_server);	// child doesn't need
			memset(bfr,0,2048);	// reset buf
			read(fd_client, bfr, 2047);	// read 2047 bytes from fd to buf
			printf("%s\n", bfr);	// print request to terminal
#ifdef DEBUG
			printf("-^request^-\n\n");
#endif
			//close unused fd
			close(cgiOutput[0]);
#ifndef DEBUG
			// the STDOUT of CGI program is redirect to cgiOutput
			//redirect the output from stdout to cgiOutput
			dup2(cgiOutput[1],STDOUT_FILENO);
#endif
			//after redirect we don't need the old fd
			close(cgiOutput[1]);
			close(fd_client);

			if (strncmp(bfr,"GET /program.cgi",16) == 0) {
				// execute cgi program
				execlp("./program.cgi","./program.cgi",NULL);
			}
			else if (strncmp(bfr,"GET /view.cgi",13) == 0) {
				// execute cgi view
				execlp("./view.cgi","./view.cgi",NULL);
			}
			else if (strncmp(bfr, "GET /insert.cgi?data=", 21) == 0) {
				inputData = strtok(bfr, " ");       // GET
				inputData = strtok(NULL, " ");      // /insert.cgi?data=...
				inputData += 17;
				execlp("./insert.cgi", "./insert.cgi", inputData, NULL);
			}
			else if (strncmp(bfr, "POST /insert.cgi", 16) == 0) {
				std::string s = bfr;
				std::string::size_type n, m;
				n = s.find("data=");
				s = s.substr(n+5);
				execlp("./insert.cgi", "./insert.cgi", s.c_str(), NULL);
			}
			else {
				write(STDOUT_FILENO, webPage, sizeof (webPage) - 1);
			}
			exit(0);
		}

		/*parent process*/
		else {
#ifdef DEBUG
			printf("this is parent process\n");
#endif
			//close unused fd
			close(cgiOutput[1]);

			// receive the message from the  CGI program
			while (read(cgiOutput[0], &c, 1) > 0) {
				// output the message to terminal
				write(fd_client, &c, 1);
			}

			// connection finish
			close(cgiOutput[0]);
#ifdef DEBUG
			printf("parent end\n\n");
#endif
			close(fd_client);
		}
	}
}