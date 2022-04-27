#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct link {
  void * val;
  char * name;
  struct link * next;
} link;
link * hm_new(void *item, char* name);
link * get_previous(link *current, char* name);
link * hm_set(link* in, char* name, void* item);
void * hm_get(link *in, char *name);
link * hm_remove(link *in, char* name);
void   hm_dump(link *in);
int    hm_length(link* in);

typedef struct link_int {
  void * val;
  int name;
  struct link_int * next;
} link_int;
link_int  * hm_set_int(link_int     *in, unsigned long long name, void* item);
void * hm_get_int(link_int     *in, unsigned long long name);
link_int  * hm_remove_int(link_int  *in, unsigned long long name);
