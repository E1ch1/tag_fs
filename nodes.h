#include <stdio.h>
#include "linked.h"
#define NODE_TYPE_FILE 0
#define NODE_TYPE_TAG 1
#define ASSOC_DEFAULT_ROOT "/" 

// The defintion of a Single node, aka. File or Tag("Dir")
typedef struct {
  const char * nodename;
  int node_type;
  int gid;
  int uid;
  int mode;
  int last_access;
  const char * content;
} node;

// So that a File or a Tag can be associated with another one more then one
// n:m
typedef struct {
  const node * node1;
  const node * node2;
} node_assoc;

int remove_specific_assoc(link * assoc_hm, node * node1, node * node2);
node_assoc * exist_specific_assoc(link * assoc_hm, node * node1, node * node2);
int remove_all_assoc(link * assoc_hm, node * node1);
int rename_all_assoc(link * assoc_hm, node * node1, char * to);
void hm_dump_link_nodeassoc(link *in);
