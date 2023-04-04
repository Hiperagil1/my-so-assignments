#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

int listDir(const char *path)
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
        
    while((entry = readdir(dir)) != NULL) {
        snprintf(filePath, 512, "%s/%s", path, entry->d_name);
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        	continue;
        else
        	printf("%s\n", filePath);
    }
    closedir(dir);
    return 0;
}


void listRec(const char *path, int suc_flag)
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
                printf("%s\n", fullPath);
                if(S_ISDIR(statbuf.st_mode)) {
                    listRec(fullPath, 1);
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
        	if(strcmp(argv[2], "recursive") == 0){        		
        		listRec(argv[3] + 5, 0);	
        	}
        	else{
        		listDir(argv[2] + 5);
        	}
    	}
    }
    return 0;
}
