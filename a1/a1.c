#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>


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

typedef struct header {
char magic[5];
short header_size;
int version;
unsigned char no_of_sections;
}Header;

typedef struct section_header {
char sect_name[13];
int sect_type;
int sect_offset;
int sect_size;
}Section_header;

void parse(const char* file) {
    FILE* fd = fopen(file, "rb");
    if (fd == NULL) {
        perror("could not open input file");
        return;
    }
    Header h;
   
    fread(h.magic, sizeof(char), 4, fd);
    fread(&h.header_size, sizeof(short), 1, fd);
    fread(&h.version, sizeof(int), 1, fd);
    fread(&h.no_of_sections, sizeof(unsigned char), 1, fd);
    
    Section_header s_h[h.no_of_sections];
    memset(s_h, 0, sizeof(s_h)); // inițializare cu zero
    for(int i =0; i<h.no_of_sections; i++){
    	fread(&s_h[i].sect_name, sizeof(char), 12, fd);
    	fread(&s_h[i].sect_type, sizeof(int), 1, fd);
    	fread(&s_h[i].sect_offset, sizeof(int), 1, fd);
    	fread(&s_h[i].sect_size, sizeof(int), 1, fd);    
    }
    fclose(fd);
    
    int ok = 1; //presupunem ca respecta formatul
    //printf("%s\n", h.magic);
    //strcpy(h.magic, "WJUo");
    if(strcmp("WJUo", h.magic) != 0){  
    	ok = 0;
    	printf("ERROR\n");
	printf("wrong magic\n");
    }
    if(h.version < 37 || h.version > 133){
    	ok = 0;
    	printf("ERROR\n");
	printf("wrong version\n");
    }
    if(h.no_of_sections < 4 || h.no_of_sections > 19){
    	ok = 0;
    	printf("ERROR\n");
	printf("wrong sect_nr\n");
    }
    	
    for(int i =0; i<h.no_of_sections; i++){
	if(s_h[i].sect_type != 65 && s_h[i].sect_type != 95 && s_h[i].sect_type != 26 && s_h[i].sect_type != 96 && s_h[i].sect_type != 77 && s_h[i].sect_type != 37){
		ok = 0;
		printf("ERROR\n");
		printf("wrong sect_types\n");
		break;
	}    
    }
    
    if(ok == 1){
    	printf("SUCCESS\n");
    	printf("version=%d\n", h.version);
    	printf("nr_sections=%d\n", h.no_of_sections);
    	for(int i =0; i<h.no_of_sections; i++){
    		
    		printf("section%d: %.*s %d %d\n", i+1, (int)strlen(s_h[i].sect_name), s_h[i].sect_name, s_h[i].sect_type, s_h[i].sect_size);

    	}
    }
}
//section_no -> numarul structurii pentru s_h ROL: size 
//line -> linia la care aflu sectiunea ceruta
//0000110100001010 -> final de secventa 

void extract(const char* file, int section_no, int line){
    FILE* fd = fopen(file, "rb");
    if (fd == NULL) {
        perror("could not open input file");
        return;
    }
    Header h;
   
    fread(h.magic, sizeof(char), 4, fd);
    fread(&h.header_size, sizeof(short), 1, fd);
    fread(&h.version, sizeof(int), 1, fd);
    fread(&h.no_of_sections, sizeof(unsigned char), 1, fd);
    
    Section_header s_h[h.no_of_sections];
    memset(s_h, 0, sizeof(s_h)); // inițializare cu zero
    for(int i =0; i<h.no_of_sections; i++){
    	fread(&s_h[i].sect_name, sizeof(char), 12, fd);
    	fread(&s_h[i].sect_type, sizeof(int), 1, fd);
    	fread(&s_h[i].sect_offset, sizeof(int), 1, fd);
    	fread(&s_h[i].sect_size, sizeof(int), 1, fd);    
    }
 
    int ok = 1;
    if(strcmp("WJUo", h.magic) != 0){  
    	ok = 0;
    }
    if(h.version < 37 || h.version > 133){
    	ok = 0;
    }
    if(h.no_of_sections < 4 || h.no_of_sections > 19){
    	ok = 0;
    }
    for(int i =0; i<h.no_of_sections; i++){
	if(s_h[i].sect_type != 65 && s_h[i].sect_type != 95 && s_h[i].sect_type != 26 && s_h[i].sect_type != 96 && s_h[i].sect_type != 77 && s_h[i].sect_type != 37){
		ok = 0;
		break;
	}    
    }
    
    if(!ok){
    	printf("ERROR\n");
    	printf("invalid file\n");
    	return;
    }
    
    if(section_no > h.no_of_sections){
    	printf("ERROR\n");
    	printf("invalid section\n");
    	return;
    }
    //setez cursorul la sectiunea ceruta	
    fseek(fd, s_h[section_no-1].sect_offset, SEEK_SET);
    //incep cu linia 1
    int cont_linii = 1;
    //aux = caracter curent
    char aux;
    //i = pozitia curenta din sectiune in raport cu inceputul si sfarsitul ei 
    int i;
    //caut linia ceruta
    ok = 0;
    for(i = 0; i<s_h[section_no-1].sect_size; i++){ //*8
    	//daca am ajuns la linia ceruta iesim
    	if(cont_linii == line){
    		break;
    	}
    	fread(&aux, sizeof(char), 1, fd);
    	if(aux == '\r'){
    		ok = 1;
    	}
    	else if(ok == 1){
    		if(aux == '\n'){
    			ok = 0;
    			cont_linii++;
    		}
    		else 
    			ok = 0;
    	}
    }

    if(cont_linii < line){
    	printf("ERROR\n");
    	printf("invalid line\n");
    	return; 
    }
    
    
    
    char sir[s_h[section_no-1].sect_size];
    int  tail = 0;
    ok = 0;
    for(int j = i; j<s_h[section_no-1].sect_size; j++){
    
    	fread(&aux, sizeof(char), 1, fd);
    	
    	if(aux == '\r')
    		ok = 1;
    	else if(ok == 1){
    		if(aux == '\n'){
    			ok = 0;
    			break;
    		}
    		else 
    		 ok = 0;
    	}
    	sir[tail] = aux;
    	tail++;
    	
    }
    
    printf("SUCCESS\n");
    for(int i = tail-1; i>=0; i--)
    	printf("%c", sir[i]);
    printf("\n");
    
    
    
    fclose(fd);	    
}

//return 0 daca nu este sf
int check(const char *fullPath){
	FILE* fd = fopen(fullPath, "rb");
    		if (fd == NULL) {
    			printf("ERROR\n");
        		perror("invalid directory path\n");
        		return -1;
    		}
    		//citim headerul
    		Header h;
    		fread(h.magic, sizeof(char), 4, fd);
    		fread(&h.header_size, sizeof(short), 1, fd);
    		fread(&h.version, sizeof(int), 1, fd);
    		fread(&h.no_of_sections, sizeof(unsigned char), 1, fd);
   		Section_header s_h[h.no_of_sections];
    		memset(s_h, 0, sizeof(s_h)); // inițializare cu zero
    		for(int i =0; i<h.no_of_sections; i++){
    			fread(&s_h[i].sect_name, sizeof(char), 12, fd);
    			fread(&s_h[i].sect_type, sizeof(int), 1, fd);
    			fread(&s_h[i].sect_offset, sizeof(int), 1, fd);
    			fread(&s_h[i].sect_size, sizeof(int), 1, fd);    
    		}
 		//presupunem ca este un fisier de tip SF valid
 		//verificam caz contrar
    		int ok = 1;
    		if(strcmp("WJUo", h.magic) != 0){  
    			ok = 0;
    		}
    		if(h.version < 37 || h.version > 133){
    			ok = 0;
    		}
    		if(h.no_of_sections < 4 || h.no_of_sections > 19){
    			ok = 0;
    		}
    		for(int i =0; i<h.no_of_sections; i++){
			if(s_h[i].sect_type != 65 && s_h[i].sect_type != 95 && s_h[i].sect_type != 26 && s_h[i].sect_type != 96 && s_h[i].sect_type != 77 && s_h[i].sect_type != 37){
				ok = 0;
				break;
			}    
    		}
    		
    		
    		if(ok == 1){
    			char aux;
    			//fseek(fd, s_h[0].sect_offset, SEEK_SET);
    			for(int j = 0; j<h.no_of_sections; j++){
    				int cont_linii = 0;
    				ok = 0;
    				for(int i = 0; i<s_h[j].sect_size*8; i++){
    					//daca am ajuns la linia ceruta iesim
    					fread(&aux, sizeof(char), 1, fd);
    					if(aux == '\r'){
    						ok = 1;
    					}	
    					else if(ok == 1){
    						if(aux == '\n'){
    							ok = 0;
    							cont_linii++;
    						}
    						else 
    							ok = 0;
    					}
    				}
    				if(cont_linii > 16){
    					fclose(fd);
    					return 1;
    					//break;
    				}
    			}
    			
    		}
    		fclose(fd);
    		return 0;
}

void findall(const char *path, int suc_flag)
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
            	if(check(fullPath))
            	    printf("%s\n", fullPath);
            	
            	if(S_ISDIR(statbuf.st_mode)) 
                    findall(fullPath, 1);
            	
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
		}
		else{
			if(recursive == 1){
				listRec(argv[path]+5, size, 0);
			}
			else{
				listDir(argv[path]+5, size);
			}
		}
    	}
    	else if(strcmp(argv[1], "parse") == 0){
    		parse(argv[2] + 5);
    	}
    	else if(strcmp(argv[1], "extract") == 0){
    		int path = -1;
    		int section_no = -1;
    		int line = -1;
    		for(int i = 2; i < argc; i++){
    			if(strncmp(argv[i], "path", 4) == 0){
				path = i;
			}
			if(strncmp(argv[i], "section", 7) == 0){
				sscanf(argv[i]+8, "%d", &section_no);
			} 
			if(strncmp(argv[i], "line", 4) == 0){		
				sscanf(argv[i]+5, "%d", &line);
			}
    		}

    		if(path == -1 || section_no == -1 || line == -1){
    			printf("ERROR\neroare citire parametri\n");
    		}
    		else{
    			extract(argv[path]+5, section_no, line);
    		}
    	}
    	else if(strcmp(argv[1], "findall") == 0){
    		findall(argv[2] + 5, 0);
    	}
    }
    return 0;
}
