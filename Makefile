
run: msh
	./msh

msh: msh.c *.h command list
	$(CC) $(CCFLAGS) -g msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CCFLAGS) -g -c command.c

list: list.c *.h
	$(CC) $(CCFLAGs) -g -c list.c

