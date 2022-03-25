#include "nodes.h"
#include <string.h>
int add_node(node *in, node into[], int length) {

  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename == NULL) {
      into[p] = *in;
      return 0;
    }
  }
  return 1;
}

int remove_node(char *nodename, node into[], int length) {
  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename != NULL) {
      if (strcmp(into[p].nodename, nodename) == 0) {
        into[p] = (node){};
        return 0;
      }
    }
  }
  return 1;
}

int search_node(char *nodename, node into[], int length) {
  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename != NULL) {
      if (strcmp(into[p].nodename, nodename) == 0) {
        return 0;
      }
    }
  }
  return 1;
}

node *get_node(char *nodename, node into[], int length) {
  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename != NULL) {
      if (strcmp(into[p].nodename, nodename) == 0) {
        return &into[p];
      }
    }
  }
  return &(node){0};
}
int get_node_fh(char *nodename, node into[], int length){
  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename != NULL) {
      if (strcmp(into[p].nodename, nodename) == 0) {
        return p;
      }
    }
  }
  return NULL;
}

void print_nodes(node into[], int length) {
  int p;
  for (p = 0; p<length;p++) {
    if (into[p].nodename != NULL) {
      printf("current nodename %s\n", into[p].nodename);
    }
  }
}

int add_assoc(char* nodename1, char* nodename2, node_assoc into[], int length) {
  int p;
  for (p=0;p<length;p++) {
    if (into[p].nodename1 == NULL && into[p].nodename2 == NULL) {
      into[p] = (node_assoc){.nodename1 = nodename1, .nodename2 = nodename2};
      return 0;
    }
  }
  return 1;
}

// returns the buffer length
// if 0 then theres no assocs found
int get_assocs(
    char* node_name, 
    node_assoc na[], int na_length,
    node nodes[], int nodes_length,
    node buffer[], int buffer_length) {
  node* ret = get_node(node_name, nodes, nodes_length);
  if (ret == 0) {
    return 0;
  }
  int p;
  int return_value = 0;
  for (p=0;p<na_length;p++) {
    if (na[p].nodename1 != NULL) {
      if (strcmp(na[p].nodename2, ret->nodename) == 0) {
        node* temp = get_node(na[p].nodename1, nodes, nodes_length);
        if (temp != 0) {
          add_node(temp, buffer, buffer_length);
          return_value++;
        }
      }
    } else if (na[p].nodename2 != NULL) {
      if (strcmp(na[p].nodename1, ret->nodename) == 0) {
        node* temp = get_node(na[p].nodename2, nodes, nodes_length);
        if (temp != 0) {
          add_node(temp, buffer, buffer_length);
          return_value++;
        }
      }
    }
  }
  return return_value;
}

// removes all occurences of this combination and returns amount removed
int remove_assoc(const char * nodename1, const char * nodename2, node_assoc na[], int length) {
  int p;
  int ret = 0;
  for(p=0;p<length;p++) {
    if (na[p].nodename1 == nodename1 && na[p].nodename2 == nodename2) {
      na[p] = (node_assoc){};
      ret++;
    }
  }
  return ret;
}
