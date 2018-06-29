#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include "collisionDetection.h"

#define NUMBER_OF_SHAPES 2000
SHAPE _shapeVector[NUMBER_OF_SHAPES];
int _shapeVectorIndex;


void doAll(char* outputFileName){

    int i, j;
    int elapsedTime = 100000; // Elapsed time in micro seconds
    FILE *outputFile;
    int collisionDetected = 1;

    outputFile = fopen(outputFileName, "w+");
    if(!outputFile){
        printf("Error creating output file \"%s\"\n", outputFileName);
        return;
    }

    loadShapes("/home/andre/github/collisionDetection/sphereData.csv", "/home/andre/github/collisionDetection/rectangleData.csv");

    collisionDetected = 1;
    while(collisionDetected){

        fprintf(outputFile, "New iteration --------------------------------\n");
        collisionDetected = 0;
        // Simulates 100 ms as elapsedTime
        updatePositions(elapsedTime);
        for(i = 0; i < _shapeVectorIndex; i++){
            for(j = 0; j < _shapeVectorIndex; j++){

                // Testing for null object
                if(!_shapeVector[i].object){
                    printf("NULL OBJECT AT %d\n", i);
                    printShape(_shapeVector[i]);
                }
                else if(!_shapeVector[j].object){
                    printf("NULL OBJECT AT %d\n", j);
                    printShape(_shapeVector[j]);
                }

                if(i != j){
                    // Checks for collisions between any pair os shapes
                    if(checkForCollision(_shapeVector[i], _shapeVector[j])){
                        // fprintf(outputFile, "Collision betweeen %d and %d\n", i, j);
                        fprintf(outputFile, "(%d, %d)\n", i, j);
                        collisionDetected = 1;
                    }
                }
            }
        }
    }
    fclose(outputFile);
    clearStructs();
}

void clearStructs(){

    int i;

    for(i = 0; i < _shapeVectorIndex; i++){
        free(_shapeVector[i].object);
    }
}

void updatePositions(int elapsedTime_uSec){

    int i;
    double elapsedTime_Sec = (double) elapsedTime_uSec / 1000000;

    for(i = 0; i < NUMBER_OF_SHAPES; i++){

        if(_shapeVector[i].object){
            // Shape exists
            switch(_shapeVector[i].type){
                case ENUM_SPHERE:{
                    SPHERE *sphere = (SPHERE*) _shapeVector[i].object;

                    sphere->centerX += _shapeVector[i].speedX * elapsedTime_Sec;
                    sphere->centerY += _shapeVector[i].speedY * elapsedTime_Sec;
                    sphere->centerZ += _shapeVector[i].speedZ * elapsedTime_Sec;
                    break;
                }
                case ENUM_RECTANGLE:{
                    RECTANGLE *rec = (RECTANGLE*) _shapeVector[i].object;

                    rec->centerX += _shapeVector[i].speedX * elapsedTime_Sec;
                    rec->centerY += _shapeVector[i].speedY * elapsedTime_Sec;
                    rec->centerZ += _shapeVector[i].speedZ * elapsedTime_Sec;
                    break;
                }
            }
        }
        else{
            // Reached first null object
            return;
        }
    }
}

void printAllShapes(){

    int i;

    for(i = 0; i < _shapeVectorIndex; i++){
        if(_shapeVector[i].object != NULL){
            fprintf(stderr, "Shape #%d\n", i);
            printShape(_shapeVector[i]);
        }
        else{
            return;
        }
    }
}

void printShape(SHAPE shape){

    SPHERE sphere;
    RECTANGLE rec;

    if(shape.type == ENUM_SPHERE){
        // Print sphere
        sphere = *(SPHERE*) shape.object;
        fprintf(stderr, "\tSphere, at (%.2f, %.2f, %.2f), radius: %.2f\n", sphere.centerX, sphere.centerY, sphere.centerY, sphere.radius);
        fprintf(stderr, "\tmoving at (%.2f, %.2f, %.2f)\n", shape.speedX, shape.speedY, shape.speedZ);
    }
    else if(shape.type == ENUM_RECTANGLE){
        // Print rectangle
        rec = *(RECTANGLE*) shape.object;
        fprintf(stderr, "\tRectangle at (%.2f, %.2f, %.2f), width: %.2f, height: %.2f, depth: %.2f\n", rec.centerX, rec.centerY, rec.centerZ, rec.width, rec.height, rec.depth);
        fprintf(stderr, "\tmoving at (%.2f, %.2f, %.2f)\n", shape.speedX, shape.speedY, shape.speedZ);
    }
}

int loadShapes(char* sphereFileName, char* recFileName){

    FILE *sphereFile, *recFile;
    float centerX, centerY, centerZ;
    float speedX, speedY, speedZ;
    float radius, width, height, depth;

    sphereFile = fopen(sphereFileName, "r");
    recFile = fopen(recFileName, "r");

    if(!sphereFile || !recFile){
        printf("Error opening files %s and %s\n", sphereFileName, recFileName);
        return 0;
    }

    _shapeVectorIndex = 0;
    // Reads the sphere data file
    while(!feof(sphereFile)){
        if(fscanf(sphereFile, "%f,%f,%f,%f,%f,%f,%f\n", &centerX, &centerY, &centerZ, &speedX, &speedY, &speedZ, &radius) == 7){
            _shapeVector[_shapeVectorIndex] = createSphere(centerX, centerY, centerZ, speedX, speedY, speedZ, radius);
            _shapeVectorIndex++;
        }
    }

    // Reads the rectangle data file
    while(!feof(recFile)) {
        if(fscanf(recFile, "%f,%f,%f,%f,%f,%f,%f,%f,%f\n", &centerX, &centerY, &centerZ, &speedX, &speedY, &speedZ, &width, &height, &depth) == 9){
            _shapeVector[_shapeVectorIndex] = createRectangle(centerX, centerY, centerZ, speedX, speedY, speedZ, width, height, depth);
            _shapeVectorIndex++;
        }
    }

    fclose(sphereFile);
    fclose(recFile);
    return (_shapeVectorIndex+1);
}

SHAPE createRectangle(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float width, float height, float depth){

    SHAPE newShape;
    RECTANGLE *newRec;

    newShape.type = ENUM_RECTANGLE;
    newRec = (RECTANGLE*) malloc(sizeof(RECTANGLE));

    newShape.speedX = speedX;
    newShape.speedY = speedY;
    newShape.speedZ = speedZ;

    newRec->centerX = centerX;
    newRec->centerY = centerY;
    newRec->centerZ = centerZ;
    newRec->width = width;
    newRec->height = height;
    newRec->depth = depth;

    newShape.object = (void*) newRec;
    return newShape;
}

SHAPE createSphere(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float radius){

    SHAPE newShape;
    SPHERE *newSphere;

    newShape.type = ENUM_SPHERE;
    newSphere = (SPHERE*) malloc(sizeof(SPHERE));

    newShape.speedX = speedX;
    newShape.speedY = speedY;
    newShape.speedZ = speedZ;

    newSphere->centerX = centerX;
    newSphere->centerY = centerY;
    newSphere->centerZ = centerZ;
    newSphere->radius = radius;

    newShape.object = (void*) newSphere;

    return newShape;
}

int checkForCollision(SHAPE shape1, SHAPE shape2){

    if(shape1.type == ENUM_SPHERE && shape2.type == ENUM_SPHERE){
        return collision_SphereSphere(*(SPHERE*) shape1.object, *(SPHERE*) shape2.object);
    }
    else if(shape1.type == ENUM_RECTANGLE && shape2.type == ENUM_RECTANGLE){
        return collision_RectangleRectangle(*(RECTANGLE*) shape1.object, *(RECTANGLE*) shape2.object);
    }
    else if(shape1.type == ENUM_SPHERE && shape2.type == ENUM_RECTANGLE){
        return collision_SphereRectangle(*(SPHERE*) shape1.object, *(RECTANGLE*) shape2.object);
    }
    else{
        return collision_SphereRectangle(*(SPHERE*) shape2.object, *(RECTANGLE*) shape1.object);
    }
}

int collision_SphereRectangle(SPHERE sphere, RECTANGLE rec){

    float distanceX, distanceY, distanceZ;
    float cornerDistance;

    distanceX = abs(sphere.centerX - rec.centerX);
    distanceY = abs(sphere.centerY - rec.centerY);
    distanceZ = abs(sphere.centerZ - rec.centerZ);

    if(distanceX > (rec.width + sphere.radius) ||
        distanceY > (rec.height + sphere.radius) ||
        distanceZ > (rec.depth + sphere.radius)){

        return 0;
    }

    if(distanceX <= rec.width ||
        distanceY <= rec.height ||
        distanceZ <= rec.depth){
        // Collision detected

        return 1;
    }

    cornerDistance = pow(distanceX - rec.width, 2) + pow(distanceY - rec.height, 2) + pow(distanceZ - rec.depth, 2);
    return (cornerDistance <= sphere.radius);
}

int collision_SphereSphere(SPHERE sphere1, SPHERE sphere2){

    float distance;

    distance = sqrt(pow(sphere1.centerX - sphere2.centerX, 2) + pow(sphere1.centerY - sphere2.centerY, 2) + pow(sphere1.centerZ - sphere2.centerZ, 2));

    if(distance > (sphere1.radius + sphere2.radius)){
        return 0;
    }

    // Collision detected
    return 1;
}

int collision_RectangleRectangle(RECTANGLE rec1, RECTANGLE rec2){

    float maxX1, maxY1, maxZ1;
    float minX1, minY1, minZ1;
    float maxX2, maxY2, maxZ2;
    float minX2, minY2, minZ2;

    maxX1 = rec1.centerX + rec1.width;
    maxY1 = rec1.centerY + rec1.height;
    maxZ1 = rec1.centerZ + rec1.depth;
    minX1 = rec1.centerX - rec1.width;
    minY1 = rec1.centerY - rec1.height;
    minZ1 = rec1.centerZ - rec1.depth;

    maxX2 = rec2.centerX + rec2.width;
    maxY2 = rec2.centerY + rec2.height;
    maxZ2 = rec2.centerZ + rec2.depth;
    minX2 = rec2.centerX - rec2.width;
    minY2 = rec2.centerY - rec2.height;
    minZ2 = rec2.centerZ - rec2.depth;

    if( maxX1 >= minX2 &&
        maxX2 >= minX1 &&
        maxY1 >= minY2 &&
        maxY2 >= minY1 &&
        maxZ1 >= minZ2 &&
        maxZ2 >= minZ1){
        // Collision detected

        return 1;
    }

    return 0;
}
