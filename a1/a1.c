#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int size_flag = 0;
int perm_flag = 0;


int listDir(const char *path, int size)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char filePath[512];

    dir = opendir(path);
    if(dir == NULL) {
    	printf("ERROR\n");
        perror("invalid directory path");
        return -1;
    }
    else
        printf("SUCCESS\n");
    struct stat statbuf;
    
    while((entry = readdir(dir)) != NULL) {
        snprintf(filePath, 512, "%s/%s", path, entry->d_name);
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        	continue;
        else if(lstat(filePath, &statbuf) == 0){
        	if(size_flag && perm_flag){
            		if (access(filePath, W_OK) == 0 && statbuf.st_size < size && !S_ISDIR(statbuf.st_mode))
            			printf("%s\n", filePath);
            	}
            	else if(size_flag){
            		if(statbuf.st_size < size && !S_ISDIR(statbuf.st_mode))
            			printf("%s\n", filePath);
            	}
            	else if(perm_flag){
            		if(access(filePath, W_OK) == 0)
            			printf("%s\n", filePath);
            	}
            	else
        		printf("%s\n", filePath);
        }
    }
    closedir(dir);
    return 0;
}


void listRec(const char *path, long long int size, int suc_flag)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[512];
    struct stat statbuf;
    //
    dir = opendir(path);
    if(dir == NULL) {
    	printf("ERROR\n");
        perror("invalid directory path");
        return;
    }
    else if (!suc_flag){
    	printf("SUCCESS\n");
    }
    	
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            snprintf(fullPath, 512, "%s/%s", path, entry->d_name);
            if(lstat(fullPath, &statbuf) == 0) {
            	if(size_flag && perm_flag){
            		if (access(fullPath, W_OK) == 0 && statbuf.st_size < size && !S_ISDIR(statbuf.st_mode))
            			printf("%s\n", fullPath);
            	}
            	else if(size_flag){
            		if(statbuf.st_size < size && !S_ISDIR(statbuf.st_mode))
            			printf("%s\n", fullPath);
            	}
            	else if(perm_flag){
            		if(access(fullPath, W_OK) == 0)
            			printf("%s\n", fullPath);
            	}
            	else
        		printf("%s\n", fullPath);
                if(S_ISDIR(statbuf.st_mode)) {
                    listRec(fullPath, size, 1);
                }
            }
        }
    }
    closedir(dir);
}



int main(int argc, char **argv){
    if(argc >= 2){
        if(strcmp(argv[1], "variant") == 0){
            printf("31771\n");
        }else if(strcmp(argv[1], "list") == 0){
        	int path = -1;
		long long int size = 0;
		int recursive = 0;
		for(int i = 2; i < argc; i++){
			if(strncmp(argv[i], "recursive", 9) == 0)
				recursive = 1;
			if(strncmp(argv[i], "size_smaller", 12) == 0){
				sscanf(argv[i]+13, "%lld", &size);
				size_flag = 1;
			}
			if(strncmp(argv[i], "has_perm_write", 14) == 0){
				perm_flag = 1;
			}
			if(strncmp(argv[i], "path", 4) == 0){
				path = i;
			}
		}
		if(path == -1){
			printf("ERROR\n");
			printf("Nu s-a gasit o cale spre un director.\n");
		}else{
			if(recursive == 1){
				listRec(argv[path]+5, size, 0);
			}
			else{
				listDir(argv[path]+5, size);
		}
		
		
		}
    	}	
    }
    return 0;
}
