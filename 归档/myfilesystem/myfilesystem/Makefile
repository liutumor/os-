output: fs.o test.o block.o
	gcc fs.o test.o block.o -o output -lm

fs.o: fs.c
	gcc  -g -c fs.c -lm

test.o: test.c
	gcc -g -c test.c

block.o: block.c
	gcc -g -c block.c

clean:
	rm *.o output