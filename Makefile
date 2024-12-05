all: compile run clean

compile: cmd.c hashtable.c
	gcc cmd.c hashtable.c -o cmd

run: 
	./cmd 

clean:
	rm cmd