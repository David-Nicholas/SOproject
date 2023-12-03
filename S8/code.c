#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <limits.h>
#include <libgen.h> 

char output[1024];
char link_output_text[1024];
char dir_output_text[1024];
char bmp_output_text[1024];
char other_output_text[1024];

void error(char *text){
	printf("%s", text);
	exit(1);
}

int is_directory(char *path){
	DIR *dir = opendir(path);
	
	if(dir != NULL){
		closedir(dir);
		return 1;
	}
	return 0;
}

int is_bmp(char *file_path){
	if(strcmp(strrchr(file_path, '.'),".bmp") == 0){
		return 1;
	}
	return 0;
}

int check_for_parameters(int argc, char **argv){
	if(argc == 3){
		if(is_directory(argv[1])){
			printf("Directorul de input validat\n");
			if(is_directory(argv[2])){
				printf("Directorul de output validat\n");
				return 1;
			}
			else{
				if(!(mkdir(argv[2], 0777))){
					printf("Directorul de output a fost creat\n");
					printf("Directorul de output a fost validat\n");
					return 1;
				}
				else{
					printf("Nu se poate crea directorul de output\n");
					return 0;
				}
			}
		}
		else{
			printf("./program <director_intrare> <-trebuie sa fie un director <director_iesire> \n");
			return 0;
		}
	}
	else{
		printf("Numar incorect de argumente\n");
		return 0;
	}
}

char* permissions_for_file(char *file_path){
	strcpy(output,"\n");
	struct stat file_stat;
	
	if(lstat(file_path, &file_stat) == 0){
		sprintf(output + strlen(output), "Drepturi de acces user: %c%c%c\n", 
			(file_stat.st_mode & S_IRUSR) ? 'R' : '-',
			(file_stat.st_mode & S_IWUSR) ? 'W' : '-',
		    	(file_stat.st_mode & S_IXUSR) ? 'X' : '-');

		sprintf(output + strlen(output), "Drepturi de acces grup: %c%c%c\n", 
			(file_stat.st_mode & S_IRGRP) ? 'R' : '-',
			(file_stat.st_mode & S_IWGRP) ? 'W' : '-',
		   	(file_stat.st_mode & S_IXGRP) ? 'X' : '-');

		sprintf(output + strlen(output), "Drepturi de acces altii: %c%c%c\n", 
			(file_stat.st_mode & S_IROTH) ? 'R' : '-',
		   	(file_stat.st_mode & S_IWOTH) ? 'W' : '-',
		   	(file_stat.st_mode & S_IXOTH) ? 'X' : '-');
	}
	
	return output;
}

char* porcess_link(char *file_path){
	strcpy(link_output_text,"\n");
	struct stat file_stat;
	
	int input_file = open(file_path, O_RDONLY);
	
	if(input_file == -1){
		printf("Eroare la fisierul %s", file_path);
		exit(1);
	}
	
	int link_size, target_size;
	
	if(lstat(file_path, &file_stat) == 0){
		link_size = file_stat.st_size;
		target_size = file_stat.st_blocks;
	}
	
	sprintf(link_output_text, "Nume fisier: %s\nDimensiune legatura: %d\nDimensiune fisier target: %d\n", basename(file_path), link_size, target_size);
	
	return link_output_text;
}

char* process_dir(char *file_path){
	strcpy(dir_output_text,"\n");
	struct stat file_stat;
	
	int input_file = open(file_path, O_RDONLY);
	
	if(input_file == -1){
		printf("Eroare la fisierul %s", file_path);
		exit(1);
	}
	
	int user_id;
	
	if(lstat(file_path, &file_stat) == 0){
		user_id = file_stat.st_uid;
	}
	
	sprintf(dir_output_text, "Nume fisier: %s\nIdentificatorul utilizatorului: %d\n", basename(file_path), user_id);
	
	return dir_output_text;
}

char* process_bmp(char *file_path){
	strcpy(bmp_output_text,"\n");
	struct stat file_stat;
	
	int input_file = open(file_path, O_RDONLY);
	
	if(input_file == -1){
		printf("Eroare la fisierul %s", file_path);
		exit(1);
	}
	
	char header[54];

	int width, height, file_size, user_id, link_count;

    	char *last_modification;
    	
    	if(lstat(file_path, &file_stat) == 0){
		read(input_file, &header, 54);
		width = *(int *)&header[18];
		height = *(int *)&header[22];
		file_size = file_stat.st_size;
		user_id = file_stat.st_uid;
		link_count = file_stat.st_nlink;
		last_modification = ctime(&file_stat.st_mtime);
	}
	
	sprintf(bmp_output_text, "Nume fisier: %s\nInaltime: %d\nLatime: %d\nDimensiune: %d\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi:%d\n", basename(file_path), height, width, file_size, user_id, last_modification, link_count);
	
	return bmp_output_text;
}

char* process_other(char *file_path){
	strcpy(other_output_text,"\n");
	struct stat file_stat;
	
	int input_file = open(file_path, O_RDONLY);
	
	if(input_file == -1){
		printf("Eroare la fisierul %s", file_path);
		exit(1);
	}	
	
	int file_size, user_id, link_count;

    	char *last_modification;
    	
    	if(lstat(file_path, &file_stat) == 0){
    		file_size = file_stat.st_size;
        	user_id = file_stat.st_uid;
        	link_count = file_stat.st_nlink;
        	last_modification = ctime(&file_stat.st_mtime);
    	}
    	
    	sprintf(other_output_text, "Nume fisier: %s\nDimensiune: %d\nIdentificatorul utilizatorului: %d\nTimpul ultimei modificari: %s\nContorul de legaturi:%d\n", basename(file_path), file_size, user_id, last_modification, link_count);
    	
    	return other_output_text;
}

void process_file(char *file_path, DIR *output_dir, char *output_dir_path){
	char file_stats[1024];
	pid_t pid = fork();
	int line_count = 0;
	int pid_status;
	struct stat file_stat;
	
	char output_file_path[1024];
	sprintf(output_file_path, "%s/%s_statistica.txt", output_dir_path, basename(file_path));
	
	if(pid < 0){
		error("Eroare fork\n");
	}
	else if(pid == 0){
			if(lstat(file_path, &file_stat) == 0){
				if(S_ISLNK(file_stat.st_mode)){
					int output_link = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					if(output_link == -1){
						printf("Eroare la fisierul %s", output_file_path);
						exit(1);
					}
					sprintf(file_stats, "%s %s", porcess_link(file_path), permissions_for_file(file_path));
					line_count = write(output_link, file_stats, strlen(file_stats));
					if(line_count == -1){
						printf("Nu s-a realizat scrierea in %s", output_file_path);
						exit(1);
					}
					close(output_link);
				}
				else if(S_ISDIR(file_stat.st_mode)){
					int output_dir = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
					if(output_dir == -1){
						printf("Eroare la fisierul %s", output_file_path);
						exit(1);
					}
					sprintf(file_stats, "%s %s", process_dir(file_path), permissions_for_file(file_path));
					line_count = write(output_dir, file_stats, strlen(file_stats));
					if(line_count == -1){
						printf("Nu s-a realizat scrierea in %s", output_file_path);
						exit(1);
					}
					close(output_dir);
				}
				else if(S_ISREG(file_stat.st_mode)){
					if(is_bmp(file_path)){
						pid_t pid_bmp = fork();
						if(pid_bmp == -1){
							error("Eroare la crearea procesului BMP\n");
						}else if(pid_bmp == 0){
						int output_bmp = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						if(output_bmp == -1){
							printf("Eroare la fisierul %s", output_file_path);
							exit(1);
						}
						sprintf(file_stats, "%s %s", process_bmp(file_path), permissions_for_file(file_path));
						line_count = write(output_bmp, file_stats, strlen(file_stats));
						if(line_count == -1){
							printf("Nu s-a realizat scrierea in %s", output_file_path);
							exit(1);
						}
						close(output_bmp);
						} else{
							int bmp_pid_status;
							waitpid(pid_bmp, &bmp_pid_status, 0);
							printf("S-a incheiat procesul BMP cu pud-ul %d si codul %d\n", pid_bmp, WEXITSTATUS(bmp_pid_status));
						}
						
					}
					else{
						int output_other = open(output_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
						if(output_other == -1){
							printf("Eroare la fisierul %s", output_file_path);
							exit(1);
						}
						sprintf(file_stats, "%s %s", process_other(file_path), permissions_for_file(file_path));
						line_count = write(output_other, file_stats, strlen(file_stats));
						if(line_count == -1){
							printf("Nu s-a realizat scrierea in %s", output_file_path);
							exit(1);
						}
						close(output_other);
					}
				}
			}		
	} else{
		pid = wait(&pid_status);
		if(WIFEXITED(pid_status)){
			printf("S-a incheiat procesul cu pid-ul %d si codul %d\n", pid, WEXITSTATUS(pid_status));
		}
	} 
}

void process_directory(DIR *input_dir, char *input_dir_path, DIR *output_dir, char *output_dir_path){
	struct dirent *file;
	while((file = readdir(input_dir)) != NULL){
		if(strcmp(file -> d_name, ".") != 0 && strcmp(file -> d_name, "..") != 0){
			char file_path[1024];
			sprintf(file_path, "%s/%s", input_dir_path, file -> d_name);
			printf("%s\n", file_path);
			process_file(file_path, output_dir, output_dir_path);
		}
	}
}

int main(int argc, char **argv){
	if(check_for_parameters(argc, argv)){
		printf("Se incepe prelucrarea datelor\n");
	}
	else{
		error("Probleme la apelare\n");
	}
	
	char *input_dir_path = argv[1];
	char *output_dir_path = argv[2];
	
	DIR *input_dir = opendir(input_dir_path);

	if(input_dir == NULL){
		error("Directorul de input nu se poate deschide\n");
	}
	
	DIR *output_dir = opendir(output_dir_path);

	if(output_dir == NULL){
		error("Directorul de output nu se poate deschide\n");
	}
	
	process_directory(input_dir, input_dir_path, output_dir, output_dir_path);
	
	closedir(input_dir);
	closedir(output_dir);
	return 0;
}
