#ifndef COLLISIONDETECTION_H
#define COLLISIONDETECTION_H

#include <time.h>
#include <stdlib.h>

enum SHAPE_TYPE{ENUM_SPHERE, ENUM_RECTANGLE};

typedef struct struct_shape{

    enum SHAPE_TYPE type;
    float speedX, speedY, speedZ;
    void *object;
}SHAPE;

typedef struct struct_sphere{

    float centerX, centerY, centerZ;
    float radius;
}SPHERE;

typedef struct struct_rectangle{

    float centerX, centerY, centerZ;
    float width, height, depth;
}RECTANGLE;

void clearStructs();
void updatePositions(int elapsedTime);
void printAllShapes();
void printShape(SHAPE shape);
int loadShapes(char* sphereFileName, char* recFileName);
SHAPE createRectangle(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float width, float height, float depth);
SHAPE createSphere(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float radius);
int checkForCollision(SHAPE shape1, SHAPE shape2);
int collision_SphereRectangle(SPHERE sphere, RECTANGLE rec);
int collision_SphereSphere(SPHERE sphere1, SPHERE sphere2);
int collision_RectangleRectangle(RECTANGLE rec1, RECTANGLE rec2);

#endif
