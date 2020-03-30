all: Host program view

CC = gcc
Host: host.c
	$(CC) -o Host host.c

program: program.c
	$(CC) -o program.cgi program.c

view: view.c
	$(CC) -o view.cgi view.c

.PHONY: clean,run
clean:
	rm Host program.cgi

test:
	./Host
