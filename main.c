#include "main.h"

#define MAX_FILE_AMOUNT 50
#define MAX_ASSOC_AMOUNT 50
int main() {
  node nodes[MAX_FILE_AMOUNT] = { 0 };
  node_assoc na[MAX_ASSOC_AMOUNT] = { 0 };

  node f = (node){.nodename = "yeah.mp4", .node_type = NODE_TYPE_FILE};
  add_node(&f, nodes, MAX_FILE_AMOUNT);

  node ff = (node){.nodename = "Music", .node_type = NODE_TYPE_TAG};
  add_node(&ff, nodes, MAX_FILE_AMOUNT);

  add_assoc("yeah.mp4", "Music", na, MAX_ASSOC_AMOUNT);

  node buffer[MAX_FILE_AMOUNT] = {0};
  int ret = get_assocs("yeah.mp4",na, MAX_ASSOC_AMOUNT, nodes, MAX_FILE_AMOUNT, buffer, MAX_FILE_AMOUNT);
  printf("%i\n", ret);

  int ret2 = remove_assoc("yeah.mp4", "Music", na, MAX_ASSOC_AMOUNT);
  printf("%i\n", ret2);

  int ret3 = get_assocs("yeah.mp4",na, MAX_ASSOC_AMOUNT, nodes, MAX_FILE_AMOUNT, buffer, MAX_FILE_AMOUNT);
  printf("%i\n", ret3);
  
  print_nodes(nodes, MAX_FILE_AMOUNT);
}
