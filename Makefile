
all:
	gcc -o collisionDetection collisionDetection.c -lm -g -Wall

clean:
	rm *.o collisionDetection
