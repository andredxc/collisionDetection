#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include "collisionDetection.h"

#define NUMBER_OF_SHAPES 2000
SHAPE _shapeVector[NUMBER_OF_SHAPES];
int _shapeVectorIndex;

int main(int argc, char** argv){

    int i, j;
    int elapsedTime = 100000; // Elapsed time in micro seconds
    FILE *outputFile;
    int collisionDetected = 1;

    if(argc < 2 || strlen(argv[1]) == 0){
        // No file name given
        printf("Usage: ./collisionDetection <outputFile>\n");
        return 0;
    }

    outputFile = fopen(argv[1], "w+");
    if(!outputFile){
        printf("Error creating output file \"%s\"\n", argv[1]);
        return 0;
    }

    loadShapes("sphereData.csv", "rectangleData.csv");
    // printAllShapes();
    // return 1;
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
    //TODO: free memory
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

void createShapes(){

    //createRectangle(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float width, float height, float depth)
    //createSphere(float centerX, float centerY, float centerZ, float speedX, float speedY, float speedZ, float radius)
    _shapeVector[0] = createRectangle(0, 0, 0, 1.8, 0, 0, 2, 2, 2);
    _shapeVector[1] = createRectangle(1, 0, 2, 0.5, 0.6, 0, 2, 2, 2);
    _shapeVector[2] = createRectangle(-5, 4, 2, 2, 0.111, 0, 1.6, 2.4, 0.9);
    _shapeVector[3] = createRectangle(4, 18, 0, 2, 1, 0, 4.8, 2, 4.8);
    _shapeVector[4] = createRectangle(4, 25, 10, 2, 0, 0, 2, 5, 1);
    _shapeVector[5] = createRectangle(24, 15, 0, 1, 0, 2, 0.1, 0.9, 0.5);
    _shapeVector[6] = createRectangle(16, 0, 0, 2, 0, 2, 2, 4, 3);
    _shapeVector[7] = createRectangle(4, 4, 4, 2, 0.3, 1, 12, 2.8, 3);
    _shapeVector[8] = createRectangle(4, 5, 9, 2, 0, -1, 1.4, 2, 3);
    _shapeVector[9] = createRectangle(4, -4, -3, 1.4, 0.4, 0.2, 2, 2.2, 3);
    _shapeVector[10] = createRectangle(4, 2, 0, 0, 0, 1, 1, 0.3, 3);
    _shapeVector[11] = createRectangle(4, 5, 0, 1.7, 0, 0, 1, 1.9, 3);
    _shapeVector[12] = createRectangle(4, 9, 0, 0, 2.3, 0, 5, 2, 3);
    _shapeVector[13] = createRectangle(4, 10, 0, 0.2, 4, -2, 9, 2.5, 3);
    _shapeVector[14] = createRectangle(4, 0, 10, 0, 2, 0, 9, 3, 3);
    _shapeVector[15] = createRectangle(4, 0, 10, 0, 0, -1, 4, 2, 3);
    _shapeVector[16] = createRectangle(4, 0, 0, 2, 0, 9, 3, 7, 3);
    _shapeVector[17] = createRectangle(4, 10, 0, 1.5, -0.9, 0, 1, 2, 3);
    _shapeVector[18] = createRectangle(4, 0, 2, 2, 0, 0, 4, 8, 3);
    _shapeVector[19] = createRectangle(10, 0, 8, 2, 7, 1, 4, 8, 3);
    _shapeVector[20] = createRectangle(4, 4, 6, 0.58, 0, 0, 4, 8, 3);

    _shapeVector[21] = createSphere(-2, 12, 3, 1, 0.3, 0, 0.9);
    _shapeVector[22] = createSphere(-2, 9, 10, -0.9, 1, 0, 2);
    _shapeVector[23] = createSphere(2, -12, 3, 10, 10, 10, 2);
    _shapeVector[24] = createSphere(-2, 0, 4, 0.1, 0, 2, 3);
    _shapeVector[25] = createSphere(0, 2, -3, 1, 0, 3, 0.648);
    _shapeVector[26] = createSphere(0.95, 2, -3, 1, 1.111, 0, 2);
    _shapeVector[27] = createSphere(4, 0.3, -1, 1, 0.5, 0, 4);
    _shapeVector[28] = createSphere(16.584, 8, 7, 0, 1, 2.5, 0.5);
    _shapeVector[29] = createSphere(1, 1, 1, -2.88, 0, 0, 3.59);
    _shapeVector[30] = createSphere(-2, 10, 1.98, 0, 0, 0, 3.987);
    _shapeVector[31] = createSphere(-4.8, 0, 0, -0.68, 4, 2, 1.948);
    _shapeVector[32] = createSphere(4, -12, 3, -0.99999, -7, 1, 3.654);
    _shapeVector[33] = createSphere(0.985, 2.94, 3.94, 2, -2, -0.999, 2.001);
    _shapeVector[34] = createSphere(-3, 10, -1, 1.84, -1, 0, 6.89);
    _shapeVector[35] = createSphere(2, 9, 3.48, 0, 0, -1.4, 1);
    _shapeVector[36] = createSphere(-1, 0.64, 4, 4, 0, 0, 0.54984);
    _shapeVector[37] = createSphere(7, 2, 12, 0, -2.54, 0, 2.95);
    _shapeVector[38] = createSphere(0.248, 2.5, 1.546, 11, 0, -5, 2.48);
    _shapeVector[39] = createSphere(-2, -4.58, -1, 10, -10, 10, 0.64);
    _shapeVector[40] = createSphere(-2, -12.16, -3, 400000, 2.548, 0, 1.651);

    _shapeVector[41] = createRectangle(4, 4, 6, 0.58, 12, 24, 4, 8, 3);
    _shapeVector[42] = createRectangle(17.8,-2.3,0.7,-2.3,-1.2,1.2,5.0,1.8,1.0);
    _shapeVector[43] = createRectangle(15.7,-5.2,-11.7,0.2,-3.6,-0.6,5.2,4.0,0.6);
    _shapeVector[44] = createRectangle(-10.6,18.4,-1.2,-1.1,3.8,-2.8,4.9,1.8,3.8);
    _shapeVector[45] = createRectangle(-14.2,-15.2,6.4,0.9,-4.2,-1.8,0.1,3.4,1.0);
    _shapeVector[46] = createRectangle(-6.1,-6.0,18.1,-2.4,0.2,-1.0,2.6,5.8,0.9);
    _shapeVector[47] = createRectangle(-9.4,-5.5,6.3,0.9,-4.0,4.7,5.5,1.1,4.8);
    _shapeVector[48] = createRectangle(17.5,11.3,7.8,2.1,4.8,-4.5,0.8,2.3,4.6);
    _shapeVector[49] = createRectangle(13.2,-9.5,-1.7,-3.7,-0.1,0.8,2.5,2.2,2.4);
    _shapeVector[50] = createRectangle(5.1,-17.4,-14.5,-4.1,1.0,3.0,4.7,3.9,2.6);
    _shapeVector[51] = createRectangle(3.3,11.4,9.9,-3.7,2.4,1.2,5.2,1.7,0.9);
    _shapeVector[52] = createRectangle(-17.6,0.6,-2.5,-3.1,-3.8,2.4,4.5,1.3,0.2);
    _shapeVector[53] = createRectangle(-10.3,10.9,-10.6,0.3,-4.4,0.3,5.8,2.7,5.8);
    _shapeVector[54] = createRectangle(1.5,13.3,7.3,-4.8,2.1,2.1,1.1,1.4,0.1);
    _shapeVector[55] = createRectangle(14.7,-11.7,-19.4,0.6,0.9,-3.1,3.3,4.7,5.9);
    _shapeVector[56] = createRectangle(-4.3,-11.9,7.6,2.7,2.4,3.8,3.8,5.2,4.3);
    _shapeVector[57] = createRectangle(1.9,-15.2,9.9,-4.9,3.6,0.2,5.3,4.8,4.1);
    _shapeVector[58] = createRectangle(7.8,-10.4,16.1,-3.8,-4.1,1.3,1.0,1.6,1.3);
    _shapeVector[59] = createRectangle(-8.6,11.8,11.1,-3.9,1.6,1.0,4.1,5.6,5.8);
    _shapeVector[60] = createRectangle(6.7,-4.0,15.6,1.5,-3.9,-3.3,0.0,5.8,0.5);
    _shapeVector[61] = createRectangle(-15.5,19.4,-10.6,3.6,-4.5,-3.3,4.9,3.3,1.6);

    _shapeVector[62] = createSphere(-17.7,-15.4,-4.3,0.9,-3.2,1.4,5.6);
    _shapeVector[63] = createSphere(0.0,-17.3,-4.8,3.5,-1.7,-1.1,1.1);
    _shapeVector[64] = createSphere(-6.5,-15.7,10.0,4.5,3.1,1.9,4.8);
    _shapeVector[65] = createSphere(0.7,9.7,-10.0,-1.2,-4.1,-1.4,5.3);
    _shapeVector[66] = createSphere(3.7,4.9,1.6,0.9,-1.5,-0.8,2.1);
    _shapeVector[67] = createSphere(12.4,-13.5,4.9,-4.9,-1.9,-2.3,5.2);
    _shapeVector[68] = createSphere(-12.1,-12.5,4.0,-4.6,0.5,1.5,3.0);
    _shapeVector[69] = createSphere(15.0,16.3,3.8,2.0,4.9,3.3,5.4);
    _shapeVector[70] = createSphere(-19.7,-16.6,11.3,0.9,3.7,-1.4,3.2);
    _shapeVector[71] = createSphere(-15.6,1.9,10.0,-1.0,4.5,-2.5,0.0);
    _shapeVector[72] = createSphere(10.8,-12.0,-1.4,-1.7,3.8,-4.3,1.6);
    _shapeVector[73] = createSphere(-5.2,-7.1,-11.9,-4.9,0.5,-3.7,0.2);
    _shapeVector[74] = createSphere(8.8,18.4,19.9,4.2,-1.0,2.8,0.8);
    _shapeVector[75] = createSphere(-5.9,12.7,-1.2,0.4,2.1,-4.7,1.7);
    _shapeVector[76] = createSphere(-1.5,9.0,-13.9,-1.5,-1.2,3.0,2.0);
    _shapeVector[77] = createSphere(-11.3,4.3,-19.0,3.4,-2.1,-3.7,3.1);
    _shapeVector[78] = createSphere(6.0,3.4,3.2,3.6,3.5,1.7,3.2);
    _shapeVector[79] = createSphere(5.3,-2.4,-5.7,2.6,2.5,-1.3,3.8);
    _shapeVector[80] = createSphere(1.3,3.8,16.9,-3.6,0.1,-2.6,0.6);
    _shapeVector[81] = createSphere(-11.5,17.9,-6.3,3.3,-0.6,-3.9,2.0);
    _shapeVector[82] = createSphere(8.1,8.6,12.1,3.8,3.9,4.3,4.7);
    _shapeVector[83] = createSphere(-4.7,-7.8,-16.7,3.8,2.2,1.6,0.2);
    _shapeVector[84] = createSphere(-1.5,-15.0,-4.4,1.8,-4.8,-0.3,1.0);
    _shapeVector[85] = createSphere(-10.3,-15.6,14.0,-1.4,-4.9,-4.1,1.3);
    _shapeVector[86] = createSphere(13.3,2.2,-2.2,-1.0,2.2,-2.5,1.8);
    _shapeVector[87] = createSphere(5.1,-8.7,-19.0,4.6,0.3,-1.2,4.3);
    _shapeVector[88] = createSphere(-9.4,16.0,-8.9,0.3,-4.0,-2.9,0.2);
    _shapeVector[89] = createSphere(-1.0,19.3,9.2,-3.7,-4.0,-3.7,5.4);
    _shapeVector[90] = createSphere(3.6,-10.0,18.5,-0.5,3.4,-1.6,5.2);
    _shapeVector[91] = createSphere(18.6,-1.8,16.5,-0.0,4.2,-2.8,4.0);
    _shapeVector[92] = createSphere(6.3,-11.1,13.1,-1.4,-0.1,2.6,5.2);
    _shapeVector[93] = createSphere(0.6,12.9,-8.3,0.5,-3.0,-2.0,2.8);
    _shapeVector[94] = createSphere(0.4,-5.4,-15.5,1.3,0.7,3.0,1.1);
    _shapeVector[95] = createSphere(-11.7,-7.8,18.9,1.1,0.3,-0.4,1.8);
    _shapeVector[96] = createSphere(-14.4,13.7,-14.7,1.4,4.7,-2.6,3.5);
    _shapeVector[97] = createSphere(-16.3,19.8,-7.9,-2.2,-3.7,4.7,4.5);
    _shapeVector[98] = createSphere(-6.7,3.7,-17.0,-3.1,2.1,-3.9,4.6);
    _shapeVector[99] = createSphere(-11.6,-6.9,4.2,-0.7,-2.2,-2.6,0.7);
    _shapeVector[100] = createSphere(6.9,0.9,-0.5,3.4,0.2,1.3,2.6);
    _shapeVector[101] = createSphere(16.6,-13.7,-9.5,2.4,1.7,1.3,5.4);
    _shapeVector[102] = createSphere(11.9,11.5,-16.3,-3.8,-2.6,-0.9,2.2);
    _shapeVector[103] = createSphere(0.8,5.4,-4.9,-4.3,-4.6,-2.3,5.6);
    _shapeVector[104] = createSphere(-1.4,3.1,9.4,3.6,-1.5,-2.1,1.2);
    _shapeVector[105] = createSphere(4.6,-5.8,-4.8,-1.3,-1.5,-1.4,5.9);
    _shapeVector[106] = createSphere(18.5,-16.1,-17.3,-1.9,1.9,-3.4,1.9);
    _shapeVector[107] = createSphere(18.0,2.1,-3.9,1.5,4.9,1.0,3.2);
    _shapeVector[108] = createSphere(19.6,-7.5,3.3,-3.0,-4.3,0.9,3.2);
    _shapeVector[109] = createSphere(-16.9,-12.1,16.0,1.1,4.7,1.1,2.9);
    _shapeVector[110] = createSphere(1.5,8.0,-11.3,-4.6,0.2,-3.6,4.9);
    _shapeVector[111] = createSphere(1.2,7.2,12.5,-4.9,2.2,-4.9,3.0);
    _shapeVector[112] = createSphere(-5.2,19.6,5.3,2.9,-4.2,0.2,1.6);
    _shapeVector[113] = createSphere(9.9,-19.8,-13.5,1.0,-0.9,1.4,1.3);
    _shapeVector[114] = createSphere(-16.7,0.3,2.7,-3.1,-0.8,-1.6,2.4);
    _shapeVector[115] = createSphere(3.1,17.3,-13.3,4.7,-0.5,-3.5,0.9);
    _shapeVector[116] = createSphere(5.7,17.5,16.4,2.4,3.3,3.2,2.8);
    _shapeVector[117] = createSphere(-1.3,14.3,-13.3,-4.1,1.1,-0.3,3.9);
    _shapeVector[118] = createSphere(-9.0,7.4,-2.9,0.0,-1.0,-3.5,5.2);
    _shapeVector[119] = createSphere(-12.2,-2.9,-1.2,-3.0,-1.9,-2.9,5.5);
    _shapeVector[120] = createSphere(12.3,2.8,6.5,-3.6,0.2,3.0,1.3);
    _shapeVector[121] = createSphere(12.4,8.7,4.2,-4.5,-0.3,3.1,1.2);
    _shapeVector[122] = createSphere(-13.9,-12.8,-8.9,0.4,-2.0,-0.4,0.6);
    _shapeVector[123] = createSphere(0.5,-13.4,7.5,2.6,3.4,-3.1,0.4);
    _shapeVector[124] = createSphere(-15.7,10.8,-18.8,-3.9,-2.7,4.4,2.9);
    _shapeVector[125] = createSphere(12.2,-19.1,-14.1,2.8,-3.8,3.3,5.9);
    _shapeVector[126] = createSphere(14.1,-16.3,-8.0,-1.1,4.4,0.2,5.8);
    _shapeVector[127] = createSphere(-2.5,11.2,7.1,-4.5,0.6,-0.1,3.6);
    _shapeVector[128] = createSphere(4.0,-18.0,2.3,-1.9,2.1,3.3,4.0);
    _shapeVector[129] = createSphere(0.8,8.3,-16.9,1.5,-4.5,-3.4,3.7);
    _shapeVector[130] = createSphere(-10.5,-2.7,-18.7,4.8,3.9,0.6,1.8);
    _shapeVector[131] = createSphere(10.9,-18.6,-18.9,-4.6,-0.2,2.9,5.8);
    _shapeVector[132] = createSphere(-11.0,18.3,9.6,2.4,-3.0,-2.3,1.1);
    _shapeVector[133] = createSphere(-8.2,4.0,16.8,1.1,-2.6,1.9,4.8);
    _shapeVector[134] = createSphere(-1.2,12.8,-6.3,1.9,1.1,4.9,1.2);
    _shapeVector[135] = createSphere(2.1,-11.5,-4.1,-2.3,2.8,3.8,3.6);
    _shapeVector[136] = createSphere(12.4,17.9,-17.3,-3.9,1.7,-0.1,2.8);
    _shapeVector[137] = createSphere(0.2,12.0,13.8,-4.6,3.3,-0.4,3.8);
    _shapeVector[138] = createSphere(16.8,10.5,12.5,3.9,3.2,-2.7,2.6);
    _shapeVector[139] = createSphere(-3.1,-19.3,18.3,-1.7,0.7,-2.6,0.3);
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
