
run: msh
	./msh

msh: msh.c *.h command list
	$(CC) $(CCFLAGS) msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CCFLAGS) -c command.c

list: list.c *.h
	$(CC) $(CCFLAGs) -c list.c

