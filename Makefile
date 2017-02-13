BINS = memprog

OBJ = memlib.o align.o init.o arenas.o blocks.o dump_struct.o assert_test.o memprog.o

memprog: $(OBJ)
	gcc -std=gnu11 -pthread -o memprog $(OBJ)

memlib.o: memlib.c
	gcc -c -std=gnu11 -Wall -Wextra memlib.c

align.o: align.c
	gcc -c -std=gnu11 -Wall -Wextra align.c

init.o: init.c
	gcc -c -std=gnu11 -Wall -Wextra init.c

arenas.o: arenas.c
	gcc -c -std=gnu11 -Wall -Wextra arenas.c

blocks.o: blocks.c
	gcc -c -std=gnu11 -Wall -Wextra blocks.c

dump_struct.o: dump_struct.c
	gcc -c -std=gnu11 -Wall -Wextra dump_struct.c

assert_test.o: assert_test.c
	gcc -c -std=gnu11 -Wall -Wextra assert_test.c
		
memprog.o: memprog.c
	gcc -c -std=gnu11 -Wall -Wextra memprog.c

clean:
	@rm -vf *.o $(BINS) 
	

