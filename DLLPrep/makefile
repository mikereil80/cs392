all:
	gcc -shared -fPIC -o my_dl.so my_dl.c -Wall -Werror -pedantic-errors
	gcc -o dl_practice dl_practice.c -Wall -Werror -pedantic-errors -ldl
clean:
	rm *.so dl_practice