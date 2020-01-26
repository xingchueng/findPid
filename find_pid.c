#define MAX_READ 100
#define MAX_LIST 100
#define MAX_PROCESSNAME_INFILE 15
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int find_pid(char* process_name);
char full_proname[256];

typedef struct {
	char pro_name[256];
	int  pid;
} found;

found found_list[MAX_LIST];

int main(int argc, int** argv){	
	int stat;

	if(argc < 2 || !strcmp(argv[1],"--help")){
			printf("usage:\n"
			 	"find_pid <process_name>     to get the pid of this process_name\n"
			    "find_pid --help             to get the command help\n");
	}else{
			
			stat = find_pid(argv[1]);
			if(stat == 0)
				printf("this process is nonexist.\n");
			else if(stat == -1)
				printf("Permission deny!\n");
			else{
				int i = 0;
				while(i < stat){
					printf("	pid(%s) = %d\n",found_list[i].pro_name, found_list[i].pid);
					++i;
				}
			}
						
	}
	return 0;
}


int find_pid(char * process_name){
	DIR *d;
	struct dirent *de;
	ssize_t readcount;
	char tmp[256];
	char tmp_name[256];
	char pro_name[256];
	char buffer[MAX_READ];
	char name_buffer[256];
	int pid;
	int tmp_pid;
	int fd,fd2;
	int count = 0;
	if((d = opendir("/proc")) == 0)
		return -1;

	while((de = readdir(d)) != 0){

		if(count == MAX_LIST){
			printf("there're more than %d processes matched,please give a longer name.\n",MAX_LIST);
			return count;
		}
		if(isdigit(de->d_name[0])){
			pid = atoi(de->d_name);
			sprintf(tmp,"/proc/%d/stat",pid);
			sprintf(tmp_name,"/proc/%d/cmdline",pid);

			fd = open(tmp,O_RDONLY);
			if(fd == -1)
				return -1;

			fd2 = open(tmp_name,O_RDONLY);
			if(fd2 == -1)
				return -1;

			readcount = read(fd2,(void*)name_buffer, 256);
			if(readcount == -1)
				return -1;
			name_buffer[readcount] = '\0';
			close(fd2);

			if(name_buffer[0]){			//in default, the full process name is keep in /proc/PID/cmdline					
				if(!strcmp(name_buffer,process_name) ){
					strcpy(found_list[count].pro_name,name_buffer);
					found_list[count].pid = pid;										
					return ++count; 
				}else if(strstr(name_buffer,process_name) != NULL){
					strcpy(found_list[count].pro_name,name_buffer);
					found_list[count].pid = pid;
					++count;
					continue;
				}	   			
			}


			readcount = read(fd, (void*)buffer, MAX_READ);
			if(readcount == -1)
				return -1;
			close(fd);

			sscanf(buffer,"%d%*[^(](%[^)]",&tmp_pid,pro_name);
			if(tmp_pid != pid)
				return -1;
			if(!strcmp(pro_name,process_name) || strstr(pro_name,process_name) != NULL){
				strcpy(found_list[count].pro_name,pro_name);
				found_list[count].pid = pid;			
				++count;
				continue;
			}else{
				if(strlen(pro_name) == MAX_PROCESSNAME_INFILE && strlen(process_name) >= MAX_PROCESSNAME_INFILE)//the max length in stat/status of the [process name] value.
					if(strstr(process_name,pro_name) != NULL){
						strcpy(found_list[count].pro_name,process_name);
						found_list[count].pid = pid;			
						++count;						
						continue;
					}
						
			}

			
		}
	}

	closedir(d);
	
	return count;
		
}

