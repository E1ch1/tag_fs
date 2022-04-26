all:
	gcc -Wall main.c nodes.c log.c linked.c -o out/fs3  `pkg-config fuse3 --cflags --libs` -D_FILE_OFFSET_BITS=64 
