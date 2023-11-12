#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>

//Verificare daca parametrul este un director
int isDirectory(char *path){
	DIR *dir = opendir(path);
	if(dir != NULL){
		closedir(dir);
		return 1;
	}
	return 0;
}

//Verificare apel
int chack_for_parameters(int argc, char **argv){
	if(argc == 2){
		if(isDirectory(argv[1])){
			printf("Se incepe prelucrea datelor\n");
			return 1;
		}else{
			printf("./program <director_intrare> <- trebuie sa fie un director\n");
			return 0;
		}	
	}	
	printf("Numar incorect de argumente\n");
	return 0;
}

//Verificare daca file-ul este un bmp
int check_for_bmp(char *fileName){
	if(strcmp(strrchr(fileName, '.'),".bmp") == 0){
		return 1;
	}
	return 0;
}

void permissions_for_files(char *fileName, char *outputFile){
	
}

//Procesare bmp
void process_bmp(char *fileName, char *outputFile){
	
}

//Procesare director
void process_directory(DIR *directory, char *directoryPath){
	struct dirent *file;
	struct stat fileStat;
	while((file = readdir(directory)) != NULL)
	{
		if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
			char filePath[1024];
            		snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, file->d_name);
			if(check_for_bmp(filePath) == 1){
				printf("%s Este bmp\n", file->d_name);
			}else{
				printf("%s Nu este bmp\n", file->d_name);
			}
		}
	}
}

int main(int argc, char **argv){
	//Verificare parametii apel:
	if(chack_for_parameters(argc, argv) == 0){
		exit(1);
	}
	
	//Creare outputFile
	int outputFile = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(outputFile == -1){
		printf("Fiserul de output nu se poate dschide sau crea\n");
		exit(1);
	}
	
	//Deschidere director
	DIR *directory = opendir(argv[1]);
	if(directory == NULL){
		printf("Directorul nu se poate deschide");
		exit(1);
	}
	
	//Incepere procesare director
	process_directory(directory, argv[1]);
	
	return 0;
}
