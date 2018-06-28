#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv){

	char outputFilePath[256];
	char compareCommand[100];
	char *args1[] = {"collisionDetection", "tempOut1", NULL};
	char *args2[] = {"collisionDetection", "tempOut2", NULL};
	FILE *tempFile1, *tempFile2, *outputFile;
	char charBuffer;
	pid_t pid;

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
		printf("Running execution 1...\n");
		if(execv("collisionDetection", args1) == -1){
			printf("1: Error running \"collisionDetection\" with args \"%s\"\n", args1[1]);
		}	
	}
	else{
		// Father process waits for the first execution to finish
		waitpid(pid, 0, 0);
	}
	printf("Done running execution 1\n");

	
	// Second execution
	pid = fork();
	if(pid == 0){
		// Executes program in child process
		printf("Running execution 2...\n");
		if(execv("collisionDetection", args2) == -1){
			printf("2: Error running \"collisionDetection\" with args \"%s\"\n", args2[1]);	
		}
	}
	else{
		// Father process waits for the second execution to finish
		waitpid(pid, 0, 0);
	}	
	printf("Done running execution 2\n");

	// Compares the two results
	snprintf(compareCommand, sizeof(compareCommand), "diff %s %s", args1[1], args2[1]);
	if(system(compareCommand) != 0){
		printf("Output files are different!\n");
	}
	else{
		printf("Output files are the same!\n");
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

	return(1);
}