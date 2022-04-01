all:
	gcc main.c nodes.c -o out/fs3 -I$(pwd)/libfuse/include -I$(pwd)/libfuse/lib -lfuse -D_FILE_OFFSET_BITS=64 -Wall
