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

int CURRENT_FD = 1;

link* nodes_hm; // All Nodes in a Hashmap
link* assoc_hm; // All assocs in a Hashmap
link* nodes_fd_hm; // All filedescriptor in a Hashmap

static int empty_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    int res = 0;
    log_debug( "Path from empty_getattr: %s", path );

	
		memset(stbuf, 0, sizeof(struct stat));
		if (strcmp(path, "/") == 0) {
			//node* n = get_node("/", nodes, MAX_FILE_AMOUNT);
      node* n = hm_get(nodes_hm, ASSOC_DEFAULT_ROOT);
      if (n == NULL) {
        return -ENOENT;
      }
			stbuf->st_mode = S_IFDIR | 0755;
      stbuf->st_uid = n->uid;
      stbuf->st_gid = n->gid;
			stbuf->st_nlink = 2;
			return 0;
		}

    char * path_new = strrchr(path, '/');
    path = path_new ? path_new + 1 : path;

    
    //char string[] = "abc/qwe/jkh";
    char *array[MAX_FILE_AMOUNT];
    int i = 0;

    array[i] = strtok(path, "/");

    while(array[i] != NULL)
        array[++i] = strtok(NULL, "/");
    
    int kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array[kk] == NULL) break;
    }
    char * filename = array[kk-1];
    
    node *ter = hm_get(nodes_hm, filename);
    if (ter == NULL) {
      return -ENOENT;
    }
    if (ter->node_type == NODE_TYPE_FILE){
        stbuf->st_mode = ter->mode;
        stbuf->st_uid = ter->uid;
        stbuf->st_gid = ter->gid;
        stbuf->st_nlink = 1;
        stbuf->st_size = 0;
        stbuf->st_size = sizeof(ter->content);
    } else {
        stbuf->st_mode = ter->mode;
        stbuf->st_uid = ter->uid;
        stbuf->st_gid = ter->gid;
        stbuf->st_nlink = 2;
    }
	return res;
}
static int empty_access(const char *path, int mask) {    
    log_debug( "Path from empty_access: %s", path );
    return 0;
}
static int empty_opendir(const char *path, struct fuse_file_info *fi) { 
    int res = 0;

    // Falls man das momentane Verzeichnis auslesen will
    if (strcmp(path, "/") == 0) {
        fi->fh = (unsigned long) 0;
        return res;
    }

    path++;
    node* nn = hm_get(nodes_hm, path);
    if (nn == NULL) {
      return -ENOENT;
    }
    hm_set_int(nodes_fd_hm, CURRENT_FD, nn);
    log_debug( "Path from empty_opendir: \n", path );
    fi->fh = (unsigned long) CURRENT_FD;
    CURRENT_FD++;
    return res;
}
static int empty_readdir(const char *path, void *dbuf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  int res = 0;
  (void) offset;
  (void) fi;
    log_debug( "Path from empty_readdir: %s", path );


  filler(dbuf, ".", NULL, 0, 0);
  filler(dbuf, "..", NULL, 0, 0);

  // Falls man das momentane Verzeichnis auslesen will
  if (strcmp(path, "/") == 0) {
    node* n = hm_get(nodes_hm, ASSOC_DEFAULT_ROOT);
    if (n == NULL) {
      return -ENOENT;
    }
    
    char* t;
    link* temp = nodes_hm;
    while(temp->next != NULL) {
      temp = temp->next;
      t = malloc(sizeof(char)*strlen(temp->name)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
      sprintf(t, "%s-%s", ASSOC_DEFAULT_ROOT, temp->name);
      node_assoc *vv = hm_get(assoc_hm, t);
      if (vv != NULL) {
        filler(dbuf, temp->name, NULL, 0, 0);
      }
    } 
    fi->fh = (unsigned long) 0; 
    return res;
  }

  path++;
  node *n = hm_get(nodes_hm, path);
  if (n == NULL) {
    return -ENOENT;
  }
  hm_set_int(nodes_fd_hm, CURRENT_FD, n);
  fi->fh = (unsigned long) CURRENT_FD;
  CURRENT_FD++;

  char* t;
  link* temp = nodes_hm;
  while(temp->next != NULL) {
    temp = temp->next;
    t = malloc(sizeof(char)*strlen(temp->name)+sizeof(char)*strlen(path)+sizeof(char)+1);
    sprintf(t, "%s-%s", path, temp->name);
    node_assoc *vv = hm_get(assoc_hm, t);
    if (vv != NULL) {
      filler(dbuf, temp->name, NULL, 0, 0);
    }
  }
  return res;
}
static int empty_mkdir(const char *path_in, mode_t mode) {
    int ret = 0;
    char * path = strdup(path_in);
    char *array[MAX_FILE_AMOUNT];
    int i = 0;

    array[i] = strtok(path, "/");

    while(array[i] != NULL)
        array[++i] = strtok(NULL, "/");
    
    int kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array[kk] == NULL) break;
    }
    char * filename = array[kk-1];

    // Dont know if i need it
    // Tests all paths beforehand with getattr
    /*
    i = 0;
    while(array[i] != NULL) {
      node *nn = hm_get(nodes_hm, array[i++]);
      if (nn == NULL) {
        return -ENOENT;
      }
    }
    */
    
    node *ff = malloc(sizeof(node));
    ff->nodename = filename; 
    ff->node_type = NODE_TYPE_TAG;
    ff->uid = getuid();
    ff->gid = getgid();
    ff->mode = S_IFDIR | 0777;
    ff->last_access = time(NULL);
    ff->content = NULL;
    hm_set(nodes_hm, ff->nodename, ff);


    if (i <= 1) {
      node_assoc *ff_na = malloc(sizeof(node_assoc));
      ff_na->nodename1 = ff->nodename;
      ff_na->nodename2 = ASSOC_DEFAULT_ROOT;

      char* tt = malloc(sizeof(char)*strlen(ff->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
      sprintf(tt, "%s-%s", ASSOC_DEFAULT_ROOT, ff->nodename);
      hm_set(assoc_hm, tt, ff_na);
    } else {
      int p = 1;
      while (array[p] != NULL) {
        char * tpath = strdup(array[p]);
        node_assoc *ff_na = malloc(sizeof(node_assoc));
        ff_na->nodename1 = ff->nodename;
        ff_na->nodename2 = tpath;

        char* tt = malloc(sizeof(char)*strlen(ff->nodename)+sizeof(char)*strlen(tpath)+sizeof(char)+1);
        sprintf(tt, "%s-%s", tpath, ff->nodename);
        hm_set(assoc_hm, tt, ff_na);

      }

    } 
    return ret;
}
static int empty_create(const char *path_in, mode_t mode, struct fuse_file_info *fi) {
    log_debug( "Path from empty_create: %s", path_in );
    (void) fi;
    int res = 0;
	
		if (strcmp(path_in, "/") == 0) {
      fi->fh = (unsigned long) 0;
			return 0;
		}
    int ret = 0;
    path_in++;
    char * path = strdup(path_in);
    char * array[MAX_FILE_AMOUNT];
    int i = 0;

    array[i] = strtok(path, "/");

    while(array[i] != NULL)
        array[++i] = strtok(NULL, "/");
    
    int kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array[kk] == NULL) break;
    }
    char * filename = array[kk-1];

    char * buffer = strdup(filename);

    node *f = malloc(sizeof(node));
    f->nodename = buffer;
    f->node_type = NODE_TYPE_FILE;
    f->uid = getuid();
    f->gid = getgid();
    f->mode = S_IFREG | 0777;
    f->last_access = time(NULL);
    f->content = NULL;
    hm_set(nodes_hm, f->nodename, f);

    if (i <= 1) {
      node_assoc *f_na = malloc(sizeof(node_assoc));
      f_na->nodename1 = f->nodename;
      f_na->nodename2 = ASSOC_DEFAULT_ROOT;

      char* t = malloc(sizeof(char)*strlen(f->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
      sprintf(t, "%s-%s", ASSOC_DEFAULT_ROOT, f->nodename);
      hm_set(assoc_hm, t, f_na);

    } else {
      // Alle Verbindungen setzen
      int i = 0;
      while(array[i] != NULL) {
        node *nn = hm_get(nodes_hm, array[i]);
        if (nn != NULL) {
          char * tpath = strdup(array[i]);
          node_assoc *f_na = malloc(sizeof(node_assoc));
          f_na->nodename1 = f->nodename;
          f_na->nodename2 = nn->nodename;

          char* t = malloc(sizeof(char)*strlen(f->nodename)+sizeof(char)*strlen(nn->nodename)+sizeof(char)+1);
          sprintf(t, "%s-%s", tpath, f->nodename);
          hm_set(assoc_hm, t, f_na);
        }
        i++;
      }
    }

    return 0;
}
static int empty_releasedir(const char *path, struct fuse_file_info *fi) {
    log_debug( "Path from empty_releasedir: %s", path );
    return 0;
}
static int empty_readlink(const char *path, char *linkbuf, size_t size) {
    log_debug( "Path from empty_readlink: %s", path );
    return 0;
}
static int empty_mknod(const char *path, mode_t mode, dev_t rdev) {
    log_debug( "Path from empty_mknod: %s", path );
    return 0;
}
static int empty_symlink(const char *from, const char *to) {
    log_debug( "Path from empty_symlink: %s", from );
    return 0;
}
static int empty_unlink(const char *path) {
    log_debug( "Path from empty_unlink: %s", path );
    return 0;
}
static int empty_rmdir(const char *path_in) {
    log_debug( "Path from empty_rmdir: %s", path_in );	
    int res = 0;
	
		if (strcmp(path_in, "/") == 0) {
			return 1;
		}
    int ret = 0;
    path_in++;
    char * path = strdup(path_in);
    char * array[MAX_FILE_AMOUNT];
    int i = 0;

    array[i] = strtok(path, "/");

    while(array[i] != NULL)
        array[++i] = strtok(NULL, "/");
    
    int kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array[kk] == NULL) break;
    }
    char * filename = array[kk-1];
    log_debug( "File from empty_rmdir: %s", path );

    node *n = hm_get(nodes_hm, filename);
    if (n == NULL) {
      return -ENOENT;
    }

    char* t;
    link* temp = nodes_hm;
    while(temp->next != NULL) {
      temp = temp->next;
      t = malloc(sizeof(char)*strlen(temp->name)+sizeof(char)*strlen(path)+sizeof(char)+1);
      sprintf(t, "%s-%s", path, temp->name);
      node_assoc *vv = hm_get(assoc_hm, t);
      if (vv != NULL) {
        int i = hm_remove(assoc_hm, t);
        if (i != 0) {
          log_warn("Could not complete deletion of %s", t);
        }
        log_info("Assoc %s has been deleted!", t);
        free(vv);
      }
    }
    i = hm_remove(nodes_hm, filename);
    if (i != 0) {
      log_warn("Could not complete deletion of %s", filename);
      return -ENOENT;
    }
    log_info("Folder %s has been deleted!", filename);
    return res;
}
static int empty_rename(const char *from, const char *to, unsigned int flags) {
    log_debug( "Path from empty_rename: %s", from );
    return 0;
}
static int empty_link(const char *from, const char *to) {
    log_debug( "Path from empty_link: %s", from );
    return 0;
}
static int empty_chmod(const char *path, mode_t mode, struct fuse_file_info *fi) {
    log_debug( "Path from empty_chmod: %s", path );
    return 0;
}
static int empty_chown(const char *path, uid_t uid, gid_t gid, struct fuse_file_info *fi) {
    log_debug( "Path from empty_chown: %s", path );
    return 0;
}
static int empty_utimens(const char *path, const struct timespec tv[2], struct fuse_file_info *fi) {
    (void) fi;
    return 0;
}
static int empty_open(const char *path, struct fuse_file_info *fi) {
    log_debug( "Path from empty_open: %s", path );
    return 0;
}
static int empty_flush(const char *path, struct fuse_file_info *fi) {
    log_debug( "Path from empty_flush: %s", path );
    return 0;
}
static int empty_fsync(const char *path, int isdatasync, struct fuse_file_info *fi) {
    log_debug( "Path from empty_fsync: %s", path );
    return 0;
}
static int empty_release(const char *path, struct fuse_file_info *fi) {
    log_debug( "Path from empty_release: %s", path );
    return 0;
}
static int empty_read(const char *path, char *rbuf, size_t size, off_t offset, struct fuse_file_info *fi) {
    log_debug( "Path from empty_read: %s", path );
    return 0;
}
static int empty_write(const char *path, const char *wbuf, size_t size, off_t offset, struct fuse_file_info *fi) {
    log_debug( "Path from empty_write: %s", path );
    return 0;
}
static int empty_statfs(const char *path, struct statvfs *buf) {
    log_debug( "Path from empty_statfs: %s", path );
    return 0;
}
static int empty_truncate(const char *path, off_t size, struct fuse_file_info *fi) {
    log_debug( "Path from empty_truncate: %s", path );
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

  insert_basics();
  test_scene();

	ret = fuse_main(args.argc, args.argv, &hello_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}

void insert_basics() {
  node *ff = malloc(sizeof(node));
  ff->nodename = "/"; 
  ff->node_type = NODE_TYPE_TAG;
  ff->uid = getuid();
  ff->gid = getgid();
  ff->mode = S_IFDIR | 0777;
  ff->last_access = time(NULL);
  ff->content = NULL;
  nodes_hm = hm_new(ff, ASSOC_DEFAULT_ROOT);

  node_assoc *root_na = malloc(sizeof(node_assoc));
  root_na->nodename1 = "";
  root_na->nodename2 = "";
  assoc_hm = hm_new(root_na, "");
  nodes_fd_hm = hm_new(ff, 0);
}

void test_scene() {
  
  node *f = malloc(sizeof(node));
  f->nodename = "yeah.mp4";
  f->node_type = NODE_TYPE_FILE;
  f->uid = getuid();
  f->gid = getgid();
  f->mode = S_IFREG | 0777;
  f->last_access = time(NULL);
  f->content = NULL;
  hm_set(nodes_hm, f->nodename, f);

  node *ff = malloc(sizeof(node));
  ff->nodename = "Music"; 
  ff->node_type = NODE_TYPE_TAG;
  ff->uid = getuid();
  ff->gid = getgid();
  ff->mode = S_IFDIR | 0777;
  ff->last_access = time(NULL);
  ff->content = NULL;
  hm_set(nodes_hm, ff->nodename, ff);

  node_assoc *ff_na = malloc(sizeof(node_assoc));
  ff_na->nodename1 = ff->nodename;
  ff_na->nodename2 = ASSOC_DEFAULT_ROOT;

  char* tt = malloc(sizeof(char)*strlen(ff->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
  sprintf(tt, "%s-%s", ASSOC_DEFAULT_ROOT, ff->nodename);
  hm_set(assoc_hm, tt, ff_na);

  node_assoc *f_na = malloc(sizeof(node_assoc));
  f_na->nodename1 = f->nodename;
  f_na->nodename2 = ASSOC_DEFAULT_ROOT;

  char* t = malloc(sizeof(char)*strlen(f->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
  sprintf(t, "%s-%s", ASSOC_DEFAULT_ROOT, f->nodename);
  hm_set(assoc_hm, t, f_na);

  node_assoc *fff_na = malloc(sizeof(node_assoc));
  fff_na->nodename1 = f->nodename;
  fff_na->nodename2 = ff->nodename;

  char* ttt = malloc(sizeof(char)*strlen(f->nodename)+sizeof(char)*strlen(ff->nodename)+sizeof(char)+1);
  sprintf(ttt, "%s-%s", ff->nodename, f->nodename);
  hm_set(assoc_hm, ttt, fff_na);
}
