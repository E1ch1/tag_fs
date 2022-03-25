#include <stdio.h>
#define NODE_TYPE_FILE 0
#define NODE_TYPE_TAG 1

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

int add_node(node *in, node into[], int length); 
int remove_node(char *nodename, node into[], int length);
void print_nodes(node into[], int length);
node* get_node(char *nodename, node into[], int length); 
int get_node_fh(char *nodename, node into[], int length); 

// So that a File or a Tag can be associated with another one more then one
// n:m
typedef struct {
  const char * nodename1;
  const char * nodename2;
} node_assoc;

int add_assoc(char* nodename1, char* nodename2, node_assoc into[], int length);

int get_assocs(
    char* node_name, 
    node_assoc na[], int na_length,
    node nodes[], int nodes_length,
    node buffer[], int buffer_length);

int remove_assoc(const char * nodename1, const char * nodename2, node_assoc na[], int length);
