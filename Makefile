
all:
	gcc -o collisionDetection collisionDetection.c -lm -g -Wall
	gcc -o collisionDetection_duplicated collisionDetection_duplicated.c -lm -g -Wall

clean:
	rm *.o collisionDetection collisionDetection_duplicated

