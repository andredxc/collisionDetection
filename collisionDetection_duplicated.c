#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv){

	char binaryPath[256], outputFilePath[256];
	char compareCommand[100];
	char* args1[] = {"tempOut1"};
	char* args2[] = {"tempOut2"};
	FILE *tempFile1, *tempFile2, *outputFile;
	char charBuffer;


	// Checks for arguments
	if(argc != 3){
		printf("Usage: ./collisionDetection_duplicated <collisionDetectionBinary> <outputFile>\n");
		return(0);
	}

	// Retrieves arguments from command line
	sscanf(argv[1], "./collisionDetection_duplicated %s %s", binaryPath, outputFilePath);
	strncpy(binaryPath, argv[1], sizeof(binaryPath));
	strncpy(outputFilePath, argv[2], sizeof(outputFilePath));

	// First execution
	if(execv(binaryPath, args1) == -1){
		printf("1: Error running \"%s\" with args \"%s\"\n", binaryPath, args1[0]);
	}

	// Second execution
	if(execv(binaryPath, args2) == -1){
		printf("2: Error running \"%s\" with args \"%s\"\n", binaryPath, args2[0]);	
	}

	// Compares the two results
	snprintf(compareCommand, sizeof(compareCommand), "diff %s %s", args1[0], args2[0]);
	if(system(compareCommand) != 0){
		printf("Output files are different!\n");
	}
	else{
		printf("Output files are the same!\n");
	}

	// Combines the two output files
	tempFile1 = fopen(args1[0], "r");
	tempFile2 = fopen(args2[0], "r");
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

	return(1);
}