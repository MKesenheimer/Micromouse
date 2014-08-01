all: main.o set.o floodfill.o motion.o maze2.o
	gcc -o Micromouse main.o set.o floodfill.o motion.o maze2.o
	
main.o: ../../src/main.c
	gcc -c -D_WIN32 -D_GNUC_ ../../src/main.c
	
set.o: ../../inc/set.h ../../inc/common.h ../../src/set.c
	gcc -c -D_WIN32 -D_GNUC_ ../../src/set.c

floodfill.o: ../../inc/floodfill.h ../../inc/set.h ../../inc/maze.h ../../inc/common.h ../../src/floodfill.c
	gcc -c -D_WIN32 -D_GNUC_ ../../src/floodfill.c
	
motion.o: ../../inc/motion.h ../../inc/common.h ../../src/motion.c
	gcc -c -D_WIN32 -D_GNUC_ ../../src/motion.c
	
maze2.o: ../../inc/maze2.h ../../inc/common.h ../../src/maze2.c
	gcc -c -D_WIN32 -D_GNUC_ ../../src/maze2.c
	
clean:
	rm -f *.o Micromouse.exe Micromouse