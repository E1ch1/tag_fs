#include "linked.h"

link* hm_new(void *item, char* name) {
  link *l = malloc(sizeof(link));
  l->name = name;
  l->val = item;
  return l;
}

link * get_last(link* current) {
  if (current->next != NULL) {
    return get_last((link*)current->next);
  }
  return current;
}

link * get_previous(link *current, char* name) {
  link *nn = current->next;
  if (nn == NULL) {
    return NULL;
  }
  if (strcmp(nn->name, name) != 0) {
    return get_last((link*)current->next);
  }
  return current;
}

void * hm_get(link *in, char *name) {
  if (strcmp(in->name, name) == 0) {
    return in->val;
  }
  if (in->next != NULL) {
    return hm_get(in->next, name);
  }
  return NULL;
}


int hm_remove(link *in, char* name) {
  link *ll = get_previous(in, name);
  if (ll == NULL) {
    return 1;
  }
  link *lll = ll->next;
  if (lll != NULL) {
    link *llll = lll->next;
    if (llll != NULL) {
      ll->next = llll;
    } else {
      ll->next = NULL;
    }
    free(lll);
  }
  return 0;
}

void hm_dump(link *in) {
  if (in->val != NULL) {
    printf("%s\n", (char*)in->val);
  } else {
    printf("%s: NO_NAME\n", in->name);
  }
  if (in->next != NULL) {
    return hm_dump(in->next);
  }
}

link * hm_set(link* in, char* name, void* item) {
  link *tmp = hm_get(in, name);
  if (tmp != NULL) {
    tmp->val = item;
    return tmp;
  }

  link *l = malloc(sizeof(link));
  l->name = name;
  l->val = item;
  link* last = (link*) get_last(in);
  last->next = l;
  return l;
}

void * hm_get_int(link *in, int name) {
  if ((int)(in->name) == name) {
    return in->val;
  }
  if (in->next != NULL) {
    return hm_get_int(in->next, name);
  }
  return NULL;
}

link * hm_set_int(link* in, int name, void* item) {
  link *tmp = hm_get_int(in, name);
  if (tmp != NULL) {
    tmp->val = item;
    return tmp;
  }

  link *l = malloc(sizeof(link));
  l->name = &name;
  l->val = item;
  link* last = (link*) get_last(in);
  last->next = l;
  return l;
}


/*
int main() {
  //node *n = malloc(sizeof(node));
  //push_node(&n);
  link* l = hm_new("Hallo", "alskdnalksd");
  hm_push(l, "World", "askdnalsdkn");

  printf("%s\n", l->val);
  char * t = hm_get(l, "askdnalsdkn");
  if (t != NULL) {
    printf("%s\n", t);
  }

  hm_remove(l, "askdnalsdkn");

  t = hm_get(l, "askdnalsdkn");
  if (t != NULL) {
    printf("%s\n", t);
  }

}
*/
