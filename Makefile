all: compile run clean

compile: cmd.c hashtable.c pipes.c
	gcc cmd.c hashtable.c pipes.c -o cmd

run: 
	./cmd 

clean:
	rm cmd