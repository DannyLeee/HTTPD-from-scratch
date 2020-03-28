all: Host program

CC = gcc
Host: host.c
	$(CC) -o Host host.c

program: program.c
	$(CC) -o program.cgi program.c

.PHONY: clean,run
clean:
	rm Host program.cgi

test:
	./Host
