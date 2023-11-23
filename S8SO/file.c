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
int check_for_parameters(int argc, char **argv){
	if(argc == 3){
		if(isDirectory(argv[1]) && isDirectory(argv[2])){
			printf("Directorul de intrare validat\n");
			printf("Directorul de iesire validat\n");
			printf("Se incepe procesarea datelor\n");
			return 1;
		}
		if(isDirectory(argv[1]) == 0){
			printf("/program <director_intrare> <- nu este un director <director_iesire> \n");
			return 0;
		}
		if(isDirectory(argv[1])){
			printf("Directorul de intrare validat\n");
			if(isDirectory(argv[2]) == 0){
				printf("./program <director_intrare> <director_iesire> <- nu este un director\n");
				printf("Se creaza <director_iesire>\n");
				if (mkdir(argv[2], 0777) == -1) {
        				perror("Eroare creare director de iesire\n");
        				return 1;
    				}
    				printf("Se incepe procesarea datelor\n");
    				return 1;
    			}
    		}
	printf("Numar incorect de argumente\n");
	return 0;
	}
}

//Verificare daca file-ul este un bmp
int check_for_bmp(char *fileName){
	if(strcmp(strrchr(fileName, '.'),".bmp") == 0){
		return 1;
	}
	return 0;
}

void permissions_for_files(char *fileName, char output[5120]){
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

void process_bmp(char *fileName, char *outputDirectoryPath){
	char output[5120];
	struct stat fileStat;
	char header[54];
	int width, height, fileSize, userId, linkCount;
    	char *lastModification;
	int inputFile = open(fileName, O_RDWR);
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
	permissions_for_files(fileName, output);
	char output_file[1024];
	sprintf(output_file, "%s/%s_statistica.txt", outputDirectoryPath, basename(fileName));
	int outputFile = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output\n");
		exit(1);
	}
	strcpy(output, "");
	
	int pixels = width * height;
	
	for(int i = 0; i < pixels; i++){
		unsigned char pixel[3];
		if (read(inputFile, pixel, sizeof(pixel)) != sizeof(pixel)) {
                        printf("Eroare la citirea pixelilor BMP\n");
                        exit(1);
                }
                
                unsigned char P_gri = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
                
                pixel[0] = P_gri;
        	pixel[1] = P_gri;
        	pixel[2] = P_gri;
                
                if (lseek(inputFile, -sizeof(pixel), SEEK_CUR) == -1) {
                        printf("Eroare la revenirea la pozitia curenta\n");
                        exit(1);
                    }
 
                if (write(inputFile, pixel, sizeof(pixel)) != sizeof(pixel)) {
                        perror("Eroare la scrierea pixelilor BMP");
                        exit(1);
                    }
	}
}

void process_dir(char *fileName, char *outputDirectoryPath){
	char output[5120];
	struct stat fileStat;
	int userId;
	int inputFile = open(fileName, O_RDONLY);
	if (stat(fileName, &fileStat) == 0){
		userId = fileStat.st_uid;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nIdentificatorul utilizatorului: %d\n", basename(fileName), userId);
	permissions_for_files(fileName, output);
	char output_file[1024];
	sprintf(output_file, "%s/%s_statistica.txt", outputDirectoryPath, basename(fileName));
	int outputFile = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output\n");
		exit(1);
	}
	strcpy(output, "");
}

void process_link(char *fileName, char *outputDirectoryPath){
	char output[5120];
	struct stat fileStat;
	int inputFile = open(fileName, O_RDONLY);
	int linkSize, targetSize;
	if (stat(fileName, &fileStat) == 0){
		linkSize = fileStat.st_size;
		targetSize = fileStat.st_blocks;
	}
	sprintf(output + strlen(output), "Nume fisier: %s\nDimensiune legatura: %d\nDimensiune fisier target: %d\n", basename(fileName), linkSize, targetSize);
	permissions_for_files(fileName, output);
	char output_file[1024];
	sprintf(output_file, "%s/%s_statistica.txt", outputDirectoryPath, basename(fileName));
	int outputFile = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output\n");
		exit(1);
	}
	strcpy(output, "");
}

void process_type(char *fileName, char *outputDirectoryPath){
	char output[5120];
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
	permissions_for_files(fileName, output);
	char output_file[1024];
	sprintf(output_file, "%s/%s_statistica.txt", outputDirectoryPath, basename(fileName));
	int outputFile = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output\n");
		exit(1);
	}
	strcpy(output, "");
}


//Procesare bmp
void process_file(char *fileName, char *outputDirectoryPath){
	struct stat fileStat;
	if (lstat(fileName, &fileStat) == 0){
        	if (S_ISLNK(fileStat.st_mode)){
			process_link(fileName, outputDirectoryPath);
		}   	
        	else if (S_ISDIR(fileStat.st_mode)){
        		process_dir(fileName, outputDirectoryPath);
        	} 
		else if (S_ISREG(fileStat.st_mode)){
			if(strcmp(strrchr(fileName, '.'),".bmp") == 0){
				process_bmp(fileName, outputDirectoryPath);
			}
			else {
				process_type(fileName, outputDirectoryPath);
			}
		}
        }     
}

//Procesare director
void process_directory(DIR *inputDirectory, char *inputDirectoryPath, DIR *outputDirectory, char *outputDirectoryPath){
	
	struct dirent *file;
	struct stat fileStat;
	while((file = readdir(inputDirectory)) != NULL)
	{
		if (strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0){
			char filePath[1024];
            		snprintf(filePath, sizeof(filePath), "%s/%s", inputDirectoryPath, file->d_name);
            		process_file(filePath, outputDirectoryPath);
		}
	}
	/*
	if(write(outputFile, output, strlen(output)) < 0)
	{
		perror("Nu se poate realiza scrierea in fisierul de output");
		exit(1);
	}
	*/
}

int main(int argc, char **argv){
	//Verificare parametii apel:
	if(check_for_parameters(argc, argv) == 0){
		exit(1);
	}
	
	//Deschidere director
	DIR *inputDirectory = opendir(argv[1]);
	if(inputDirectory == NULL){
		printf("Directorul nu se poate deschide\n");
		exit(1);
	}
	
	DIR *outputDirectory = opendir(argv[2]);
	if(outputDirectory == NULL){
		printf("Directorul nu se poate deschide\n");
		exit(1);
	}
	
	//Incepere procesare director
	process_directory(inputDirectory, argv[1], outputDirectory, argv[2]);
	
	return 0;
}
