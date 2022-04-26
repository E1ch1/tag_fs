all:
	gcc -Wall main.c nodes.c linked.c -o out/fs3  `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 
	#-I$(shell pwd)/libfuse/include -I$(shell pwd)/libfuse/lib 
