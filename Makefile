override CFLAGS += -Wall -Werror -pedantic

# running it from make will actually cause make
# to be the parent process, and thus it will
# be suspended if you send SIGTSTP, which kinda
# interferes with the program.
# run: msh
# 	./msh

msh: msh.c *.h command list built_in handlers
	$(CC) $(CFLAGS) -g msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CFLAGS) -g -c command.c

list: list.c *.h
	$(CC) $(CFLAGS) -g -c list.c

built_in: built_in.c *.h
	$(CC) $(CFLAGS) -g -c built_in.c

handlers: handlers.c *.h
	$(CC) $(CFLAGS) -g -c handlers.c

