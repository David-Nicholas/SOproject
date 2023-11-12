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
#include <libgen.h> 

char output[5120];

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

void permissions_for_files(char *fileName){
	struct stat fileStat;
	if (stat(fileName, &fileStat) == 0){
		sprintf(output + strlen(output), "Drepturi de acces user: %c%c%c\n", 
			(fileStat.st_mode & S_IRUSR) ? 'R' : '-',
			(fileStat.st_mode & S_IWUSR) ? 'W' : '-',
		    	(fileStat.st_mode & S_IXUSR) ? 'X' : '-');
		    	
		sprintf(output + strlen(output), "Drepturi de acces grup: %c%c%c\n", 
			(fileStat.st_mode & S_IRGRP) ? 'R' : '-',
			(fileStat.st_mode & S_IWGRP) ? 'W' : '-',
		   	(fileStat.st_mode & S_IXGRP) ? 'X' : '-');
			
		sprintf(output + strlen(output), "Drepturi de acces altii: %c%c%c\n\n\n____________________________________________________________\n\n", 
			(fileStat.st_mode & S_IROTH) ? 'R' : '-',
		   	(fileStat.st_mode & S_IWOTH) ? 'W' : '-',
		   	(fileStat.st_mode & S_IXOTH) ? 'X' : '-');
	}
}

void process_bmp(char *fileName){
	struct stat fileStat;
	char header[54];
	int width, height, fileSize, userId, linkCount;
    	char *lastModification;
	int inputFile = open(fileName, O_RDONLY);
	if (stat(fileName, &fileStat) == 0){
		read(inputFile, &header, 54);
		width = *(int *)&header[18];
        	height = *(int *)&header[22];
        	fileSize = fileStat.st_size;
        	userId = fileStat.st_uid;
        	lastModification = ctime(&fileStat.st_mtime);
        	linkCount = fileStat.st_nlink;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %d\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi:%d\n", basename(fileName), height, width, fileSize, userId, lastModification, linkCount);
	permissions_for_files(fileName);
}

void process_dir(char *fileName){
	struct stat fileStat;
	int userId;
	int inputFile = open(fileName, O_RDONLY);
	if (stat(fileName, &fileStat) == 0){
		userId = fileStat.st_uid;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nIdentificatorul utilizatorului: %d\n", basename(fileName), userId);
	permissions_for_files(fileName);
}

void process_link(char *fileName){
	struct stat fileStat;
	int inputFile = open(fileName, O_RDONLY);
	int linkSize, targetSize;
	if (stat(fileName, &fileStat) == 0){
		linkSize = fileStat.st_size;
		targetSize = fileStat.st_blocks;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nDimensiune legatura: %d\nDimensiune fisier target: %d\n", basename(fileName), linkSize, targetSize);
	permissions_for_files(fileName);
}

void process_type(char *fileName){
	struct stat fileStat;
	int fileSize, userId, linkCount;
    	char *lastModification;
	int inputFile = open(fileName, O_RDONLY);
	if (stat(fileName, &fileStat) == 0){
        	fileSize = fileStat.st_size;
        	userId = fileStat.st_uid;
        	lastModification = ctime(&fileStat.st_mtime);
        	linkCount = fileStat.st_nlink;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nDimensiune: %d\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi:%d\n", basename(fileName), fileSize, userId, lastModification, linkCount);
	permissions_for_files(fileName);
}


//Procesare bmp
void process_file(char *fileName){
	struct stat fileStat;
	if (lstat(fileName, &fileStat) == 0){
        	if (S_ISLNK(fileStat.st_mode)){
			process_link(fileName);
		}   	
        	else if (S_ISDIR(fileStat.st_mode)){
        		process_dir(fileName);
        	} 
		else if (S_ISREG(fileStat.st_mode)){
			if(strcmp(strrchr(fileName, '.'),".bmp") == 0){
				process_bmp(fileName);
			}
			else {
				process_type(fileName);
			}
		}
        }     
}

//Procesare director
void process_directory(DIR *directory, char *directoryPath, int outputFile){
	
	struct dirent *file;
	struct stat fileStat;
	while((file = readdir(directory)) != NULL)
	{
		if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
			char filePath[1024];
            		snprintf(filePath, sizeof(filePath), "%s/%s", directoryPath, file->d_name);
            		process_file(filePath);
		}
	}
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output");
		exit(1);
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
		printf("Fiserul de output nu se poate dschide sau creat\n");
		exit(1);
	}
	
	//Deschidere director
	DIR *directory = opendir(argv[1]);
	if(directory == NULL){
		printf("Directorul nu se poate deschide");
		exit(1);
	}
	
	//Incepere procesare director
	process_directory(directory, argv[1], outputFile);
	
	if(close(outputFile) < 0)
	{
		perror("Nu se poate inchide fisierul de output");
		exit(1);
	}
	
	return 0;
}
