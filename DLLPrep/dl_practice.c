#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
	void *handle;
	int (*add)(int, int);
	
	handle = dlopen("./my_dl.so", RTLD_NOW);
	if (!handle) {
		fprintf(stderr, "Error: Failed to create handle: %s.\n", dlerror());
		return EXIT_FAILURE;
	}
	
	*(int**)(&add) = dlsym(handle, "add");
}