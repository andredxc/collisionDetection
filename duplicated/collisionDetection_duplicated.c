#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include "collisionDetection.h"

#define OUT1 "/home/andre/github/collisionDetection/duplicated/tempFile1"
#define OUT2 "/home/andre/github/collisionDetection/duplicated/tempFile2"
#define SDC_LOG "duplicated_collisionDetection_sdc_count.txt"

int main(int argc, char** argv){

	char outputFilePath[256];
	char compareCommand[256];
	FILE *tempFile1, *tempFile2, *outputFile, *sdcLogFile;
	char charBuffer;
	int sdcFound = 0;
	
	/*
	struct timeval t1, t2;
	double elapsedTime;
	gettimeofday(&t1, NULL);	
	*/

	// Checks for arguments
	if(argc != 2){
		printf("Usage: ./collisionDetection_duplicated <outputFilePath>\n");
		return(0);
	}

	// Retrieves arguments from command line
	strncpy(outputFilePath, argv[1], sizeof(outputFilePath));

	// First execution
	// doAll(OUT1);

	// Second execution
	// doAll(OUT2);

	// Compares the two results
	snprintf(compareCommand, sizeof(compareCommand), "diff %s %s", OUT1, OUT2);
	if(system(compareCommand) != 0){
		// Output files are different, SDC detected
		sdcFound = 1;
	}	

	// Combines the two output files
	tempFile1 = fopen(OUT1, "r");
	tempFile2 = fopen(OUT2, "r");
	outputFile = fopen(outputFilePath, "w");

	if(!tempFile1 || !tempFile2 || !outputFile){
		printf("Error opening output files\n");
	}

	while((charBuffer = fgetc(tempFile1)) != EOF){
		fputc(charBuffer, outputFile);
	}
	while((charBuffer = fgetc(tempFile2)) != EOF){
		fputc(charBuffer, outputFile);
	}

	fclose(tempFile1);
	fclose(tempFile2);
	fclose(outputFile);

	// Adds SDC to log file
	if(sdcFound){
		sdcLogFile = fopen(SDC_LOG, "a");
		if(sdcLogFile){
			fprintf(sdcLogFile, "SDC happened\n");
			fclose(sdcLogFile);
		}
	}

	/*
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec);
	printf("Execution time: %f\n", elapsedTime);
	*/

	return(1);
}