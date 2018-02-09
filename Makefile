override CFLAGS += -Wall -Werror -pedantic

run: msh
	./msh

msh: msh.c *.h command list built_in
	$(CC) $(CFLAGS) -g msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CFLAGS) -g -c command.c

list: list.c *.h
	$(CC) $(CFLAGS) -g -c list.c

built_in: built_in.c *.h
	$(CC) $(CFLAGS) -g -c built_in.c

