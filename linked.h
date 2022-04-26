#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct link {
  void * val;
  void * name;
  struct link * next;
} link;

link * hm_new(void *item, char* name);
link * get_previous(link *current, char* name);
link * hm_set(link* in, char* name, void* item);
void * hm_get(link *in, char *name);
int    hm_remove(link *in, char* name);
void   hm_dump(link *in);
link * hm_set_int(link* in, int name, void* item);
void * hm_get_int(link *in, int name);
