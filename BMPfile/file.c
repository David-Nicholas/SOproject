#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h> 
//./ file testfile.bmp

int check_for_parameters(int argc, char **argv){
	if(argc == 2){
		if(strcmp(strrchr(argv[1], '.'),".bmp") == 0){
			printf("Se proceseaza datele\n");
			return 1;
		}else{ 
			printf("Fiserul nu este unul .bmp\n");
		}
	}
	else {
		printf("Numar incorect de argumente\n");
	}
	return 0;
}

int main(int argc, char **argv){
	if(check_for_parameters(argc, argv) == 0){
		exit(1);
	}
	
	int inputFile = open(argv[1], O_RDONLY);
	if(inputFile == -1){
		printf("Fiserul de input nu se poate dschide\n");
		exit(1);
	}
	
	int outputFile = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if(outputFile == -1){
		printf("Fiserul de output nu se poate dschide\n");
		exit(1);
	}
	
	
	char header[54];
	read(inputFile, &header, 54);
	int width = *(int *)&header[18];
	int height = *(int *)&header[22];
	char fileName[50];
	strcpy(fileName, argv[1]);
	
	struct stat fileStat;
	fstat(inputFile, &fileStat);
	int fileSize = fileStat.st_size;
	int userId = fileStat.st_uid;
	char *lastModification = ctime(&fileStat.st_mtime);
	int linkCount = fileStat.st_nlink;
	
	char output[10000];
	sprintf(output, "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %d\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi:%d\n", fileName, height, width, fileSize, userId, lastModification, linkCount);
	
	sprintf(output + strlen(output), "Drepturi de acces user: %c%c%c\n", 
		(fileStat.st_mode & S_IRUSR) ? 'R' : '-',
		(fileStat.st_mode & S_IWUSR) ? 'W' : '-',
            	(fileStat.st_mode & S_IXUSR) ? 'X' : '-');
            	
        sprintf(output + strlen(output), "Drepturi de acces grup: %c%c%c\n", 
	        (fileStat.st_mode & S_IRGRP) ? 'R' : '-',
	        (fileStat.st_mode & S_IWGRP) ? 'W' : '-',
           	(fileStat.st_mode & S_IXGRP) ? 'X' : '-');
        	
        sprintf(output + strlen(output), "Drepturi de acces altii: %c%c%c\n", 
		(fileStat.st_mode & S_IROTH) ? 'R' : '-',
           	(fileStat.st_mode & S_IWOTH) ? 'W' : '-',
           	(fileStat.st_mode & S_IXOTH) ? 'X' : '-');
           	
        if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output");
		exit(1);
	}
           	
	if(close(inputFile) < 0)
	{
		perror("Nu se poate inchide fisierul de input");
		exit(1);
	}
	
	if(close(outputFile) < 0)
	{
		perror("Nu se poate inchide fisierul de output");
		exit(1);
	}
	
	return 0;
}
