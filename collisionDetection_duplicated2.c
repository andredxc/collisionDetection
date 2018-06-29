#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char** argv){

	char outputFilePath[256];
	char compareCommand[100];
	char *args1[] = {"/home/andre/github/collisionDetection/collisionDetection", "/home/andre/github/collisionDetection/tempOut1", NULL};
	char *args2[] = {"/home/andre/github/collisionDetection/collisionDetection", "/home/andre/github/collisionDetection/tempOut2", NULL};
	char sdcLogFileName[60] = {"duplicated_collisionDetection_sdc_count.txt"};
	FILE *tempFile1, *tempFile2, *outputFile, *sdcLogFile;
	char charBuffer;
	pid_t pid;
	int sdcCount, sdcFound = 0;
	char fileContentBuf[50];
	
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
	pid = fork();
	if(pid == 0){
		// Executes program in child process
		if(execv("/home/andre/github/collisionDetection/collisionDetection", args1) == -1){
			printf("1: Error running \"collisionDetection\" with args \"%s\"\n", args1[1]);

		}	
	}
	else{
		// Father process waits for the first execution to finish
		waitpid(pid, 0, 0);
	}

	// Second execution
	pid = fork();
	if(pid == 0){
		// Executes program in child process
		if(execv("/home/andre/github/collisionDetection/collisionDetection", args2) == -1){
			printf("2: Error running \"collisionDetection\" with args \"%s\"\n", args2[1]);	
		}
	}
	else{
		// Father process waits for the second execution to finish
		waitpid(pid, 0, 0);
	}	

	// Compares the two results
	snprintf(compareCommand, sizeof(compareCommand), "diff %s %s", args1[1], args2[1]);
	if(system(compareCommand) != 0){
		// Output files are different, SDC detected
		sdcFound = 1;
	}	

	// Combines the two output files
	tempFile1 = fopen(args1[1], "r");
	tempFile2 = fopen(args2[1], "r");
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
		sdcLogFile = fopen(sdcLogFileName, "r");
		if(sdcLogFile && fgets(fileContentBuf, 100, sdcLogFile) && atoi(fileContentBuf)){
			// Read something
			sdcCount = atoi(fileContentBuf) + 1;
			fclose(sdcLogFile);
		}
		else{
			// No file
			sdcCount = 1;
		}
		sdcLogFile = fopen(sdcLogFileName, "w");
		fprintf(sdcLogFile, "%d", sdcCount);
		fclose(sdcLogFile);
	}

	/*
	gettimeofday(&t2, NULL);
	elapsedTime = (t2.tv_sec - t1.tv_sec);
	printf("Execution time: %f\n", elapsedTime);
	*/

	return(1);
}