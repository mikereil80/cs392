#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

int main() {
	int my_fd, bytes_written;
	char *initial_contents = "this is the contents of myfile\n";
	
	if ((my_fd = open("myfile.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
		fprintf(stderr, "Failed to create file: %s\n", strerror(errno));
		return EXIT_FAILURE;    
	}    
	bytes_written = write(my_fd, initial_contents, strlen(initial_contents));
	close(my_fd);
	
	if (bytes_written < 0) {
		fprintf(stderr, "Failed to write to file: %s\n", strerror(errno));
		return EXIT_FAILURE;    
	}
	// remaining code snippets will be put in here
	struct stat statbuf;
	
	// note that I had to reference statbuf to fill it. Why is this?
	if(stat("myfile.txt", &statbuf)<0){
		fprintf(stderr, "Failed to get information o the file: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	
	printf("Size of file: %lu characters\n", statbuf.st_size);
	
	printf("File owner's UID: %u\n", statbuf.st_uid);
	
	printf("Last access time: %s", ctime(&statbuf.st_atime));
	printf("Last modification time: %s", ctime(&statbuf.st_mtime));
	
	printf("Permisiions of file: %d\n", statbuf.st_mode);
	
	int user_read_permissions=statbuf.st_mode & S_IRUSR;
	
	int perms[]={S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
	int permission_valid;
	printf("-"); // the is to print the file type bit, as displayed in `$ ls -l `.
	for(int i=0; i<9; i+=3){
		permission_valid = statbuf.st_mode & perms[i];
		if(permission_valid){
			printf("r");
		}
		else{
			printf("-");
		}
		permission_valid = statbuf.st_mode & perms[i+1];
		if(permission_valid){
			printf("w");
		}
		else{
			printf("-");
		}
		permission_valid = statbuf.st_mode & perms[i+2];
		if(permission_valid){
			printf("x");
		}
		else{
			printf("-");
		}
	}
	printf("\n");
}