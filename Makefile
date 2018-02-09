
run: msh
	./msh

msh: msh.c *.h command
	$(CC) $(CCFLAGS) msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CCFLAGS) -c command.c


