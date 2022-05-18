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
#include <math.h>

#define MAX_FILE_AMOUNT 256
#define MAX_ASSOC_AMOUNT 256

int CURRENT_FD = 1;

link* nodes_hm; // All Nodes in a Hashmap
link* assoc_hm; // All assocs in a Hashmap
link* nodes_fd_hm; // All filedescriptor in a Hashmap


// Source @: https://localcoder.org/generate-random-64-bit-integer
signed rand256() {
    static unsigned const limit = RAND_MAX - RAND_MAX % 256;
    unsigned result = rand();
    while ( result >= limit ) {
        result = rand();
    }
    return result % 256;
}

unsigned long rand64bits() {
    unsigned long results = 0ULL;
    for ( int count = 8; count > 0; -- count ) {
        results = 256U * results + rand256();
    }
    results >> 1;
    return results;
}

unsigned long getNextFH (char* name) {
  while(1) {
    unsigned long temp = rand64bits();
    char* tt = hm_get_int(nodes_fd_hm, temp);
    if (tt == NULL) {
      link *l = hm_set_int(nodes_fd_hm, temp, name);
      return temp;
    }
  }
}

int get_filename_and_paths(char** buffer[], char * path) {

  if (strcmp(path, "/") == 0) {
    return 0;
  }

  int i = 0;

  buffer[i] = strtok(path, "/");

  while(buffer[i] != NULL)
      buffer[++i] = strtok(NULL, "/");
  
  int kk = 0;
  for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
      if (buffer[kk] == NULL) break;
  }
  return i;
}



static int empty_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
  (void) fi;
  int res = 0;
  log_debug( "Path from empty_getattr: %s", path );
  hm_dump(nodes_hm);
  hm_dump(assoc_hm);

  memset(stbuf, 0, sizeof(struct stat));
  char * array[MAX_FILE_AMOUNT];
  int ret_1 = get_filename_and_paths(&array, path);

  if (ret_1 < 1) {
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

  char * filename = array[ret_1-1];
  array[ret_1-1] = NULL;
  node *n1 = hm_get(nodes_hm, filename);
  if (n1 == NULL) {
    return -ENOENT;
  }


  int ii = 0;
  while(array[ii] != NULL) {

    node *n2 = hm_get(nodes_hm, array[ii]);
    if (n2 == NULL) { 
    }
    link * pp = exist_specific_assoc(assoc_hm, n1, n2);
    if (pp == NULL) {
      return -ENOENT;
    }
    ii++;
  }

  if (n1 == NULL) {
    return -ENOENT;
  }
  if (n1->node_type == NODE_TYPE_FILE){
      stbuf->st_mode = n1->mode;
      stbuf->st_uid = n1->uid;
      stbuf->st_gid = n1->gid;
      stbuf->st_nlink = 1;
      stbuf->st_size = 0;
      stbuf->st_size = sizeof(n1->content);
  } else {
      stbuf->st_mode = n1->mode;
      stbuf->st_uid = n1->uid;
      stbuf->st_gid = n1->gid;
      stbuf->st_nlink = 2;
  }
  return res;
}

static int empty_opendir(const char *path, struct fuse_file_info *fi) { 
  int res = 0;

  // Falls man das momentane Verzeichnis auslesen will
  if (strcmp(path, "/") == 0) {
    fi->fh = (unsigned long) getNextFH(ASSOC_DEFAULT_ROOT);
    return res;
  }

  path++;
  node* nn = hm_get(nodes_hm, path);
  if (nn == NULL) {
    return -ENOENT;
  }
  fi->fh = (unsigned long) getNextFH(path);
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
    return res;
  }

  path++;
  node *n = hm_get(nodes_hm, path);
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
      filler(dbuf, temp->name, NULL, 0, 0);
    }
  }
  return res;
}
static int empty_mkdir(const char *path, mode_t mode) {
    int ret = 0;
    char *array[MAX_FILE_AMOUNT];
    int i = get_filename_and_paths(&array, path);
    char * filename = array[i-1];

    char * temp = malloc(sizeof(char)*strlen(filename)+1);
    strcpy(temp, filename);
    
    node *new_node = malloc(sizeof(node));
    new_node->nodename = temp;
    new_node->node_type = NODE_TYPE_TAG;
    new_node->uid = getuid();
    new_node->gid = getgid();
    new_node->mode = S_IFDIR | 0777 | mode;
    new_node->last_access = time(NULL);
    new_node->content = NULL;
    hm_set(nodes_hm, new_node->nodename, new_node);

    // Every Directory is in Root!!
    node *n2 = hm_get(nodes_hm, ASSOC_DEFAULT_ROOT);
    node_assoc *new_node_na = malloc(sizeof(node_assoc));
    new_node_na->node1 = new_node;
    new_node_na->node2 = n2;

    char* tt = malloc(sizeof(char)*strlen(new_node->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
    sprintf(tt, "%s-%s", ASSOC_DEFAULT_ROOT, new_node->nodename);
    hm_set(assoc_hm, tt, new_node_na);

    if (i > 1) {
      int p = 1;
      while (array[p] != NULL) {
        node *n2 = hm_get(nodes_hm, array[p]);

        node_assoc *n1_n2_na = malloc(sizeof(node_assoc));
        n1_n2_na->node1 = n2;
        n1_n2_na->node2 = new_node;

        char* tt = malloc(sizeof(char)*strlen(new_node->nodename)+sizeof(char)*strlen(n2->nodename)+sizeof(char)+1);
        sprintf(tt, "%s-%s", n2->nodename, new_node->nodename);
        hm_set(assoc_hm, tt, n1_n2_na);
      }
    } 
    return ret;
}
static int empty_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void) fi;
    log_debug( "Path from empty_create: %s", path );
    
    int res = 0;
    char *array[MAX_FILE_AMOUNT];
    int i = get_filename_and_paths(&array, path);
    char * filename = array[i-1];
    char * temp = malloc(sizeof(char)*strlen(filename)+1);
    strcpy(temp, filename);

    node *new_node = malloc(sizeof(node));
    new_node->nodename = temp;
    new_node->node_type = NODE_TYPE_FILE;
    new_node->uid = getuid();
    new_node->gid = getgid();
    new_node->mode = S_IFREG | 0777;
    new_node->last_access = time(NULL);
    new_node->content = NULL;

    // Not all Files in root!!
    if (i <= 1) {
      node *n2 = hm_get(nodes_hm, ASSOC_DEFAULT_ROOT);
      node_assoc *new_node_na = malloc(sizeof(node_assoc));
      new_node_na->node1 = new_node;
      new_node_na->node2 = n2;

      char* t = malloc(sizeof(char)*strlen(new_node->nodename)+sizeof(char)*strlen(ASSOC_DEFAULT_ROOT)+sizeof(char)+1);
      sprintf(t, "%s-%s", ASSOC_DEFAULT_ROOT, new_node->nodename);
      hm_set(assoc_hm, t, new_node_na);

    } else {
      // Alle Verbindungen setzen
      int i = 0;
      while(array[i] != NULL) {
        node *n2 = hm_get(nodes_hm, array[i]);

        if (n2 != NULL) {
          node_assoc *new_node_n2_na = malloc(sizeof(node_assoc));
          new_node_n2_na->node1 = new_node->nodename;
          new_node_n2_na->node2 = n2->nodename;

          char* t = malloc(sizeof(char)*strlen(new_node->nodename)+sizeof(char)*strlen(n2->nodename)+sizeof(char)+1);
          sprintf(t, "%s-%s", n2->nodename, new_node->nodename);
          hm_set(assoc_hm, t, new_node_n2_na);
        }

        i++;
      }
    }
    hm_set(nodes_hm, new_node->nodename, new_node);

    return 0;
}
static int empty_rmdir(const char *path) {
    log_debug( "Path from empty_rmdir: %s", path );	
     
    int res = 0;
    char *array[MAX_FILE_AMOUNT];
    int i = get_filename_and_paths(&array, path);
    char * filename = array[i-1];
    node *n1 = hm_get(nodes_hm, filename);
    if (n1 == NULL) {
      return -ENOENT;
    }

    int ret_1 = remove_all_assoc(assoc_hm, n1);
    log_debug("Removed %i assocs for %s", ret_1, filename);

    link * pp = hm_remove(nodes_hm, filename);
    if (pp == NULL) {
      log_warn("Could not complete deletion of %s", filename);
      return -ENOENT;
    }

    free(pp);
    log_info("Folder %s has been deleted!", filename);
    log_debug("Amount of nodes: %i", hm_length(nodes_hm));
    return res;
}
/*
static int empty_unlink(const char *path_in) {
    // Delete a File
    log_debug( "Path from empty_unlink: %s", path_in );
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
    int ii = 0;
    while(array[ii] != NULL) {
      int k = remove_specific_assoc(filename, array[ii]);
      ii++;
    }
    if (ii == 0) {
      int k = remove_specific_assoc(filename, ASSOC_DEFAULT_ROOT);
      link * ll = hm_remove(nodes_hm, filename);
      if (ll == NULL) {
        printf("Could not delete File: %s", filename);
      }
    }

    return 0;
}

static int empty_access(const char *path, int mask) { 
    log_debug( "Path from empty_access: %s", path );
    log_debug( "Mask from empty_access: %i", mask);
    // Allow All because i have no idea rn
    return 0;
}
static int empty_rename(const char *from, const char *to, unsigned int flags) {
    // Beim umbenenneen aka. mv filea fileb
    log_debug( "Path from empty_rename from: %s", from );
    log_debug( "Path from empty_rename to: %s", to );

    int res = 0;
    char * filename_from;
    char * filename_to; 
    char * array_from[MAX_FILE_AMOUNT];
    char * array_to[MAX_FILE_AMOUNT];
    char * path_from;
    char * path_to;


    // From PART =========================== 0


    // From PART =========================== 1
    //
    // To PART   =========================== 0
    // FILENAME PARTS ====================== 0
    path_from = strdup(from);
    int i = 0;
    array_from[i] = strtok(path_from, "/");
    while(array_from[i] != NULL)
        array_from[++i] = strtok(NULL, "/");
    int kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array_from[kk] == NULL) break;
    }
    filename_from = array_from[kk-1];
    log_debug("Filename_from is: %s", filename_from);

    path_to = strdup(to);
    i = 0;
    array_to[i] = strtok(path_to, "/");
    while(array_to[i] != NULL)
        array_to[++i] = strtok(NULL, "/");
    kk = 0;
    for (kk = 0; kk < MAX_FILE_AMOUNT; kk++) {
        if (array_to[kk] == NULL) break;
    }
    filename_to = array_to[kk-1];
    log_debug("Filename_to is: %s", filename_to);
    // FILENAME PARTS ====================== 1

    // RENAME AND CONQUER
    node * link_from = hm_get(nodes_hm, filename_from);
    if (link_from == NULL) {
      return -ENOENT;
    }

    node * link_to = hm_get(nodes_hm, filename_to);
    if (link_to != NULL) {
      // FIND AND CONQUER
      remove_all_assoc(filename_to);
      remove_specific_assoc(filename_to, ASSOC_DEFAULT_ROOT);
      link * aa = hm_remove(nodes_hm, filename_to);
      if (aa == NULL) {
        log_error("Konnte den alten Link nicht entfernen!!");
      }
      log_debug("Komplette Säuberung von %s", filename_to);
      free(link_to);
    }

    if (link_from != NULL) {
      printf("%s\n", link_from->nodename);
      printf("%s\n", filename_to);
      rename_all_assoc(link_from->nodename, filename_to);
      link * ll = hm_get_link(nodes_hm, link_from->nodename);
      ll->name = strdup(filename_to);
      link_from->nodename = strdup(filename_to);
    }

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
    // update timestamp of a File eg. touch filea
    (void) fi;
    return 0;
}
static int empty_open(const char *path, struct fuse_file_info *fi) {
    // get FH for a file, eg. cat filea
    log_debug( "Path from empty_open: %s", path );
    return 0;
}
static int empty_flush(const char *path, struct fuse_file_info *fi) {
    // Flush the output pipe
    log_debug( "Path from empty_flush: %s", path );
    return 0;
}
static int empty_release(const char *path, struct fuse_file_info *fi) {
    // Release a FH
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


// Functions that CAN be implemented but dont have to. Will be last
static int empty_mknod(const char *path, mode_t mode, dev_t rdev) {
    // Create a normal File, will NOT be called if create is defined
    log_debug( "Path from empty_mknod: %s", path );
    return 0;
}
static int empty_readlink(const char *path, char *linkbuf, size_t size) {
    log_debug( "Path from empty_readlink: %s", path );
    return 0;
}
static int empty_symlink(const char *from, const char *to) {
    log_debug( "Path from empty_symlink: %s", from );
    return 0;
}
static int empty_link(const char *from, const char *to) {
    log_debug( "Path from empty_link: %s", from );
    return 0;
}
static int empty_fsync(const char *path, int isdatasync, struct fuse_file_info *fi) {
    log_debug( "Path from empty_fsync: %s", path );
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
*/
/* TODO: Irgendwas stimmt net mit den FH's
static int empty_releasedir(const char *path, struct fuse_file_info *fi) {
    log_debug( "empty_releasedir: Path from empty_releasedir: %s", path );
    log_debug( "empty_releasedir: FH: %u", fi->fh );
    hm_remove_int(nodes_fd_hm, fi->fh);
    log_debug( "empty_releasedir: Amount of open Filehandles: %i", hm_length(nodes_fd_hm) );

    return 0;
}
*/
static struct fuse_operations hello_oper = {
		//.init       = empty_init, 
		.getattr    = empty_getattr, 
		.opendir    = empty_opendir, 
		.readdir    = empty_readdir, 
		.mkdir      = empty_mkdir,
		.create     = empty_create,
		.rmdir      = empty_rmdir, 
/*
		.releasedir = empty_releasedir, 
		.access     = empty_access, 
		.readlink   = empty_readlink,
		.mknod      = empty_mknod,
		.symlink    = empty_symlink, 
		.unlink     = empty_unlink, 
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
  */
};

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	args.argv[0][0] = '\0';

  test_scene();

	ret = fuse_main(args.argc, args.argv, &hello_oper, NULL);
	fuse_opt_free_args(&args);
	return ret;
}

void test_scene() {
  node *root = malloc(sizeof(node));
  root->nodename = "/"; 
  root->node_type = NODE_TYPE_TAG;
  root->uid = getuid();
  root->gid = getgid();
  root->mode = S_IFDIR | 0777;
  root->last_access = time(NULL);
  root->content = NULL;
  nodes_hm = hm_new(root, ASSOC_DEFAULT_ROOT);

  node *yeah = malloc(sizeof(node));
  yeah->nodename = "yeah.mp4";
  yeah->node_type = NODE_TYPE_FILE;
  yeah->uid = getuid();
  yeah->gid = getgid();
  yeah->mode = S_IFREG | 0777;
  yeah->last_access = time(NULL);
  yeah->content = NULL;
  hm_set(nodes_hm, yeah->nodename, yeah); 

  node *music = malloc(sizeof(node));
  music->nodename = "Music"; 
  music->node_type = NODE_TYPE_TAG;
  music->uid = getuid();
  music->gid = getgid();
  music->mode = S_IFDIR | 0777;
  music->last_access = time(NULL);
  music->content = NULL;
  hm_set(nodes_hm, music->nodename, music);

  node *nope = malloc(sizeof(node));
  nope->nodename = "nope.mp3";
  nope->node_type = NODE_TYPE_FILE;
  nope->uid = getuid();
  nope->gid = getgid();
  nope->mode = S_IFREG | 0777;
  nope->last_access = time(NULL);
  nope->content = NULL;
  hm_set(nodes_hm, nope->nodename, nope);

  node_assoc *root_na = malloc(sizeof(node_assoc));
  root_na->node1 = root;
  root_na->node2 = root;
  assoc_hm = hm_new(root_na, "/-/");
  nodes_fd_hm = hm_new(root, 0);

  node_assoc *yeah_na = malloc(sizeof(node_assoc));
  yeah_na->node1 = yeah;
  yeah_na->node2 = root;
  hm_set(assoc_hm, "/-yeah.mp4", yeah_na);

  node_assoc *music_na = malloc(sizeof(node_assoc));
  music_na->node1 = music;
  music_na->node2 = root;
  hm_set(assoc_hm, "/-Music", music_na);

  node_assoc *nope_na = malloc(sizeof(node_assoc));
  nope_na->node1 = nope;
  nope_na->node2 = root;
  hm_set(assoc_hm, "/-nope.mp3", nope_na);

  node_assoc *music_yeah = malloc(sizeof(node_assoc));
  music_yeah->node1 = yeah;
  music_yeah->node2 = music;
  hm_set(assoc_hm, "Music-yeah.mp4", music_yeah);
}
