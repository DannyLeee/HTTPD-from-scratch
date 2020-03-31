all: Host program view insert

CC = gcc
Host: host.c
	$(CC) -o Host host.c

program: program.c
	$(CC) -o program.cgi program.c

view: view.c
	$(CC) -o view.cgi view.c

insert: insert.c
	$(CC) -o insert.cgi insert.c

.PHONY: clean,run
clean:
	rm Host program.cgi

test:
	./Host
