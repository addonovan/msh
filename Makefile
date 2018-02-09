
run: msh
	./msh

msh: msh.c *.h command list built_in
	$(CC) $(CCFLAGS) -g msh.c *.o -o msh

command: command.c *.h
	$(CC) $(CCFLAGS) -g -c command.c

list: list.c *.h
	$(CC) $(CCFLAGS) -g -c list.c

built_in: built_in.c *.h
	$(CC) $(CCFLAGS) -g -c built_in.c

