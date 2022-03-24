#define FUSE_USE_VERSION 31
#define MAX_FILE_AMOUNT 256
#define MAX_TAG_AMOUNT 256
#define MAX_FILE_TAG_AMOUNT 512
#define MAX_TAG_DEPTH 16

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <search.h>

int CURR_FH_DIR_ID = 1;
int CURR_FH_FILE_ID = 1;

static void *empty_init(struct fuse_conn_info *conn, struct fuse_config *cfg) {
    // Entfernt die conn da sie net gebraucht wird
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

typedef struct {
    const char * filename;
    int flags;
} file;

typedef struct {
    const char * tagname;
    int flags;
} tag;

typedef struct {
    file * file_part;
    tag * tag_part;
    int flags;
} file_tag;

static const file files[MAX_FILE_AMOUNT];
static const tag tags[MAX_TAG_AMOUNT];
static const file_tag file_tags[MAX_TAG_DEPTH];
//file_tag[0] = 

// int (*getattr) (const char *, struct stat *, struct fuse_file_info *fi);
// int (*create) (const char *, mode_t, struct fuse_file_info *);
// int (*opendir) (const char *, struct fuse_file_info *);
// int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *, enum fuse_readdir_flags);
// int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
// int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);
// int (*open) (const char *, struct fuse_file_info *);
// int (*rename) (const char *, const char *, unsigned int flags);
// int (*mkdir) (const char *, mode_t);

// ######################################################
int create_file(const char *path) {

}
// ######################################################

static int empty_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
	int res = 0;

    printf( "Path from getattr: %s\n", path ); //printing each token
	
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return res;
	} 

    path++;
    printf( "Komme ich hier rein2: %s\n", path ); //printing each token
    printf( "sizeof tags: %li\n", sizeof(tags)/sizeof(tag) ); //printing each token
	
    int p;
	for(p=0;p<sizeof(tags)/sizeof(tag);p++) {
        if (&tags[p] == NULL) continue;
		if (&tags[p] == NULL) { 
            if (strcpy(tags[p].tagname, path)) {
                stbuf->st_mode = S_IFDIR | 0755;
                stbuf->st_nlink = 2;
                return 0;  
            }
            res = p;
        }
	}
    printf( "sizeof tags: %li\n", sizeof(tags)/sizeof(tag) ); //printing each token

    for(p=0;p<sizeof(tags)/sizeof(file);p++) {
        if (&files[p] == NULL) continue;
		if (&files[p] == NULL) { 
            if (strcpy(files[p].filename, path)) {
              	stbuf->st_mode = S_IFREG | 0444;
                stbuf->st_nlink = 1;
                stbuf->st_size = 0;
                return res;  
            }
            res = p;
        }
	}
    create_file(path);
	return 0;
}
static int empty_access(const char *path, int mask) {    
    printf( "Path from empty_access: %s\n", path );
    return 0;
}
static int empty_opendir(const char *path, struct fuse_file_info *fi) { 
    printf( "Path from empty_opendir: %s\n", path );
    return 0;
}
static int empty_readdir(const char *path, void *dbuf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    printf( "Path from empty_readdir: %s\n", path );
    return 0;
}
static int empty_releasedir(const char *path, struct fuse_file_info *fi) {
    printf( "Path from empty_releasedir: %s\n", path );
    return 0;
}
static int empty_readlink(const char *path, char *linkbuf, size_t size) {
    printf( "Path from empty_readlink: %s\n", path );
    return 0;
}
static int empty_mkdir(const char *path, mode_t mode) {
    printf( "Path from empty_mkdir: %s\n", path );
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
    int res;
    printf( "Path from empty_utimens: %s\n", path );
    /* don't use utime/utimes since they follow symlinks */
    res = utimensat(0, path, tv, AT_SYMLINK_NOFOLLOW);
    if (res == -1)
            return -ENOENT;
    return 0;
    /*
    (void)fi;
    fi->fh = create_file(path);
    printf( "Path from empty_utimens: %s\n", path );
    return 0;
    */
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
		.init       = empty_init, 
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

	// Creates a weird hashmap thing i believe
	hcreate(MAX_FILE_AMOUNT);

	ret = fuse_main(args.argc, args.argv, &hello_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}
