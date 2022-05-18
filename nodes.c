#include "nodes.h"
#include <string.h>
#include "log.h"

int remove_specific_assoc(link * assoc_hm, node * node1, node * node2) {
  if (node1 == node2)
    return 1;

  char * t = malloc(sizeof(char)*strlen(node1->nodename)+sizeof(char)*strlen(node2->nodename)+sizeof(char)+1);
  sprintf(t, "%s-%s", node1->nodename, node2->nodename);
  node_assoc *vv = hm_get(assoc_hm, t);

  if (vv != NULL) {
    log_debug("Trying to delete %s", t);
    link *ii = hm_remove(assoc_hm, t);
    if (ii==NULL) {
      log_warn("Something whent Wrong with code: %i", ((link *)vv)->name);
      return 1;
    }
    log_info("Assoc %s has been deleted!", t);
    free(ii);
  }
  return 0;
}
node_assoc * exist_specific_assoc(link * assoc_hm, node * node1, node * node2 ){
  node_assoc * ret = NULL;
  if (node1 == node2)
    return NULL;

  char * t = malloc(sizeof(char)*strlen(node2->nodename)+sizeof(char)*strlen(node1->nodename)+sizeof(char)+1);
  sprintf(t, "%s-%s", node2->nodename, node1->nodename);
  node_assoc *vv = hm_get(assoc_hm, t);
  printf("Searching for this term: %s\n", t);

  if (vv != NULL) {
    return vv;
  }

  return ret;
}

int remove_all_assoc(link * assoc_hm, node * node1) {

  int ret = 0;
  link * temp = assoc_hm;
  while(temp != NULL) {

    node_assoc *na = (node_assoc*)(temp->val);
    if (na->node1 == node1 || na->node2 == node1) {
      node *n1 = (node*)na->node1;
      node *n2 = (node*)na->node2;
      printf("Trying to remove: %s-%s\n", n1->nodename, n2->nodename);
      int found = remove_specific_assoc(assoc_hm, n2, n1);
      if (found == 0) {
        free(na);
      }
      ret++;
    }
    if (temp->next == NULL) {
      printf("The next is null%s\n", temp->name);
    }
    temp = temp->next;
  }

  return ret;
}

int rename_all_assoc(link * assoc_hm, node * node1, char * to) {
  if (node1->nodename == to) 
    return 0;

  int ret = 0;
  link * temp = assoc_hm;
  while(temp != NULL) {
    node_assoc *na = (node_assoc*)(temp->val);
    if (na->node1 == node1 || na->node2 == node1) {
      
      char * t = malloc(sizeof(char)*strlen(node1->nodename)+sizeof(char)*strlen(to)+sizeof(char)+1);
      sprintf(t, "%s-%s", node1->nodename, to);
      free(temp->name);
      temp->name = t;

      ret++;
    }
    temp = temp->next;
  }
  return ret;
}

void hm_dump_link_nodeassoc(link *in) {
  link * temp = in;
  while(temp != NULL) {
    node_assoc * as = temp->val;
    node * n1 = (node*)as->node1;
    node * n2 = (node*)as->node2;
    if (n1 != NULL && n2 != NULL)
      printf("Name of Link: %s to %s\n", n1->nodename, n2->nodename);
    temp = temp->next;
  }
}
