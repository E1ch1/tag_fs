#define FUSE_USE_VERSION 31
#include "main.h"
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <search.h>

#define MAX_FILE_AMOUNT 256
#define MAX_ASSOC_AMOUNT 256

int CURR_FH_DIR_ID = 1;
int CURR_FH_FILE_ID = 1;
node nodes[MAX_FILE_AMOUNT] = { 0 };
node_assoc na[MAX_ASSOC_AMOUNT] = { 0 };

/*
int main() {

  node buffer[MAX_FILE_AMOUNT] = {0};
  int ret = get_assocs("yeah.mp4",na, MAX_ASSOC_AMOUNT, nodes, MAX_FILE_AMOUNT, buffer, MAX_FILE_AMOUNT);
  printf("%i\n", ret);

  int ret2 = remove_assoc("yeah.mp4", "Music", na, MAX_ASSOC_AMOUNT);
  printf("%i\n", ret2);

  int ret3 = get_assocs("yeah.mp4",na, MAX_ASSOC_AMOUNT, nodes, MAX_FILE_AMOUNT, buffer, MAX_FILE_AMOUNT);
  printf("%i\n", ret3);
  
  print_nodes(nodes, MAX_FILE_AMOUNT);
}

static void *empty_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
{
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}
*/

static int empty_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    int res = 0;
    printf( "Path from empty_getattr: %s\n", path );
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return res;
	}

    char * path_new = strrchr(path, '/');
    path = path_new ? path_new + 1 : path;

    node ter = *get_node(path, nodes, MAX_FILE_AMOUNT);
    if (ter.nodename == NULL) {
        res = -ENOENT;
    } else {
        if (ter.node_type == NODE_TYPE_FILE){
            stbuf->st_mode = ter.mode;
            stbuf->st_uid = ter.uid;
            stbuf->st_gid = ter.gid;
            stbuf->st_nlink = 1;
            stbuf->st_size = 0;
            stbuf->st_size = sizeof(ter.content);
        } else {
            stbuf->st_mode = ter.mode;
            stbuf->st_uid = ter.uid;
            stbuf->st_gid = ter.gid;
		    stbuf->st_nlink = 2;
        }
    }
	return res;
}
static int empty_access(const char *path, int mask) {    
    printf( "Path from empty_access: %s\n", path );
    return 0;
}
static int empty_opendir(const char *path, struct fuse_file_info *fi) { 
    int res = 0;
    printf( "Path from empty_opendir: %s\n", path );

    // Falls man das momentane Verzeichnis auslesen will
    if (strcmp(path, "/") == 0) {
        fi->fh = (unsigned long) 0;
        return res;
    }

    path++;
    int fh = get_node_fh(path, nodes, MAX_FILE_AMOUNT);
    if (fh == NULL || nodes[fh].node_type == NODE_TYPE_FILE) {
        res = -ENOENT;
    } else {
        fi->fh = (unsigned long) fh;
    }
    return res;
}
static int empty_readdir(const char *path, void *dbuf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    int res = 0;
    (void) offset;
    (void) fi;
	
    filler(dbuf, ".", NULL, 0);
	filler(dbuf, "..", NULL, 0);

    // Falls man das momentane Verzeichnis auslesen will
    if (strcmp(path, "/") == 0) {
        int t;
        for (t=0;t<MAX_FILE_AMOUNT;t++) {
            if (nodes[t].nodename != NULL && nodes[t].node_type == NODE_TYPE_TAG) {
                //printf( "Found the following: %s\n", nodes[t].nodename );
                filler(dbuf, nodes[t].nodename, NULL, 0);
            }
        }
        return res;
    }

    path++;
    int fh = get_node_fh(path, nodes, MAX_FILE_AMOUNT);
    if (fh == NULL || nodes[fh].node_type == NODE_TYPE_FILE) {
        res = -ENOENT;
    } else {
        fi->fh = (unsigned long) fh;
        node buffer[MAX_FILE_AMOUNT];
        int amount = get_assocs(path, na, MAX_ASSOC_AMOUNT, nodes, MAX_FILE_AMOUNT, buffer, MAX_FILE_AMOUNT);
        if (amount != 0) {
            int t;
            for (t=0;t<amount;t++) {
                filler(dbuf, buffer[t].nodename, NULL, 0);
            }
        }
    }
    return res;
}
static int empty_mkdir(const char *path, mode_t mode) {
    path++;
    char * buffer = strdup(path);
    node xx = (node){
        .nodename = buffer,
        .node_type = NODE_TYPE_TAG,
        .uid = getuid(),
        .gid = getgid(),
        .mode = S_IFDIR | 0777,
        .last_access = time(NULL),
        .content = NULL
    };
    int ret = add_node(xx, nodes, MAX_FILE_AMOUNT);
    return ret;
}
static int empty_releasedir(const char *path, struct fuse_file_info *fi) {
    printf( "Path from empty_releasedir: %s\n", path );
    return 0;
}
static int empty_readlink(const char *path, char *linkbuf, size_t size) {
    printf( "Path from empty_readlink: %s\n", path );
    return 0;
}
static int empty_mknod(const char *path, mode_t mode, dev_t rdev) {
    printf( "Path from empty_mknod: %s\n", path );
    return 0;
}
static int empty_symlink(const char *from, const char *to) {
    printf( "Path from empty_symlink: %s\n", from );
    return 0;
}
static int empty_unlink(const char *path) {
    printf( "Path from empty_unlink: %s\n", path );
    return 0;
}
static int empty_rmdir(const char *path) {
    printf( "Path from empty_rmdir: %s\n", path );
    return 0;
}
static int empty_rename(const char *from, const char *to, unsigned int flags) {
    printf( "Path from empty_rename: %s\n", from );
			
    return 0;
}
static int empty_link(const char *from, const char *to) {
    printf( "Path from empty_link: %s\n", from );
    return 0;
}
static int empty_chmod(const char *path, mode_t mode, struct fuse_file_info *fi) {
    printf( "Path from empty_chmod: %s\n", path );
    return 0;
}
static int empty_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi) {
    printf( "Path from empty_chown: %s\n", path );
    return 0;
}
static int empty_utimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi) {
  (void) fi;
  return 0;
}
static int empty_open(const char *path, struct fuse_file_info *fi) {
    printf( "Path from empty_open: %s\n", path );
    return 0;
}
static int empty_flush(const char *path, struct fuse_file_info *fi) {
    printf( "Path from empty_flush: %s\n", path );
    return 0;
}
static int empty_fsync(const char *path, int isdatasync, struct fuse_file_info *fi) {
    printf( "Path from empty_fsync: %s\n", path );
    return 0;
}
static int empty_release(const char *path, struct fuse_file_info *fi) {
    printf( "Path from empty_release: %s\n", path );
    return 0;
}
static int empty_read(const char *path, char *rbuf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf( "Path from empty_read: %s\n", path );
    return 0;
}
static int empty_write(const char *path, const char *wbuf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf( "Path from empty_write: %s\n", path );
    return 0;
}
static int empty_statfs(const char *path, struct statvfs *buf) {
    printf( "Path from empty_statfs: %s\n", path );
    return 0;
}
static int empty_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    printf( "Path from empty_create: %s\n", path );
    return 0;
}
static int empty_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    printf( "Path from empty_truncate: %s\n", path );
    return 0;
}
static struct fuse_operations hello_oper = {
		//.init       = empty_init, 
		.getattr    = empty_getattr, 
		.access     = empty_access, 
		.opendir    = empty_opendir, 
		.readdir    = empty_readdir, 
		.releasedir = empty_releasedir, 
		.readlink   = empty_readlink,
		.mknod      = empty_mknod,
		.mkdir      = empty_mkdir,
		.symlink    = empty_symlink, 
		//.unlink     = empty_unlink, 
		.rmdir      = empty_rmdir, 
		.rename     = empty_rename, 
		.link       = empty_link, 
		.chmod      = empty_chmod, 
		.chown      = empty_chown, 
		.truncate   = empty_truncate,
		.utimens    = empty_utimens,
		.open       = empty_open,
		.flush      = empty_flush,
		.fsync      = empty_fsync,
		.release    = empty_release,
		.read       = empty_read,
		.write      = empty_write,
		.statfs     = empty_statfs,
		.create     = empty_create,
};

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	args.argv[0][0] = '\0';

    node pp = (node){.nodename = NULL, .node_type = NODE_TYPE_TAG};
    add_node(pp, nodes, MAX_FILE_AMOUNT);

    node f = (node){
        .nodename = "yeah.mp4", 
        .node_type = NODE_TYPE_FILE,
        .uid = getuid(),
        .gid = getgid(),
        .mode = S_IFREG | 0777,
        .last_access = time(NULL),
        .content = NULL
    };
    add_node(f, nodes, MAX_FILE_AMOUNT);

    node ff = (node){
        .nodename = "Music", 
        .node_type = NODE_TYPE_TAG,
        .uid = getuid(),
        .gid = getgid(),
        .mode = S_IFDIR | 0777,
        .last_access = time(NULL),
        .content = NULL
    };
    add_node(ff, nodes, MAX_FILE_AMOUNT);

    add_assoc("yeah.mp4", "Music", na, MAX_ASSOC_AMOUNT);



	ret = fuse_main(args.argc, args.argv, &hello_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}
