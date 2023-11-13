#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <libgen.h> 

int isDirectory(char *path){
	DIR *dir = opendir(path);
	if(dir != NULL){
		closedir(dir);
		return 1;
	}
	return 0;
}

int check_for_parameters(int argc, char **argv){
	if(argc == 3){
		if(isDirectory(argv[1]) && isDirectory(argv[2]))
		{
			printf("Directorul de intrare validat\n");
			printf("Directorul de iesire validat\n");
			printf("Se incepe procesarea datelor\n");
			return 1;
		}
		if(isDirectory(argv[1]) == 0 && isDirectory(argv[2]) == 0)
		{
			printf("Amblele argumente nu sunt directoare\n");
			return 0;
		}
		if(isDirectory(argv[1])){
			printf("Directorul de intrare validat\n");
			if(isDirectory(argv[2])){
				printf("Directorul de iesire validat\n");
				printf("Se incepe procesarea datelor\n");
				return 1;
			}else{
				printf("./program <director_intrare> <director_iesire> <- nu este un director\n");
				return 0;
			}
		}else{
			printf("./program <director_intrare> <- nu este un director <director_iesire>\n");
				return 0;
		}
	}
	printf("Numar incorect de argumente\n");
	return 0;
}

int get_file_count(DIR * directory)
{
	int file_count = 0;
	struct dirent *file;
	
	while((file = readdir(directory)) != NULL){
		if((file->d_type == DT_REG) || (file->d_type == DT_DIR) || (file->d_type == DT_LNK)){
			file_count++;
		}
	}
	return file_count;
}

int main (int argc, char ** argv){
	if(check_for_parameters(argc, argv) == 0){
		exit(1);
	}
	
	DIR *inputDirectory = opendir(argv[1]);
	if(inputDirectory == NULL){
		printf("Directorul nu se poate deschide");
		exit(1);
	}
	
	DIR *outputDirectory = opendir(argv[2]);
	if(outputDirectory == NULL){
		printf("Directorul nu se poate deschide");
		exit(1);
	}
	
	
	
	int file_count = get_file_count(inputDirectory);
	
	printf("%d\n", file_count);
	
	return 0;
}
