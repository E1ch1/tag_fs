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
link_int * get_last_int(link_int* current) {
  if (current->next != NULL) {
    return get_last_int((link_int*)current->next);
  }
  return current;
}

link * get_previous(link *current, char* name) {
  link *nn = current;
  while(nn->next != NULL) {
    link *nnn = nn->next;
    if (strcmp(nnn->name, name) == 0) {
      return nn;
    }
    nn = nnn;
  }
  return NULL;
}

link_int * get_previous_int(link_int *current, unsigned long long name) {
  link_int *nn = current->next;
  while(nn->next != NULL) {
    link_int *nnn = nn->next;
    if (nnn->name == name) {
      return nn;
    }
    nn = nnn;
  }
  return NULL;
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


link * hm_remove(link *in, char* name) {
  link *ll = get_previous(in, name);
  if (ll == NULL) {
    return NULL;
  }
  link *lll = ll->next;
  if (lll != NULL) {
    link *llll = lll->next;
    if (llll != NULL) {
      ll->next = llll;
    } else {
      ll->next = NULL;
    }
    return lll;
  }
  return NULL;
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

void * hm_get_int(link_int *in, unsigned long long name) {
  if ((int)(((*in).name)) == name) {
    return in->val;
  }
  if (in->next != NULL) {
    return hm_get_int(in->next, name);
  }
  return NULL;
}

link_int * hm_set_int(link_int* in, unsigned long long name, void* item) {
  link_int *tmp = hm_get_int(in, name);
  if (tmp != NULL) {
    tmp->val = item;
    return tmp;
  }

  link_int *l = malloc(sizeof(link));
  l->name = name;
  l->val = item;
  link_int* last = get_last_int(in);
  last->next = l;
  return l;
}

link_int * hm_remove_int(link_int *in, unsigned long long name) {
  link_int *ll = get_previous_int(in, name);
  if (ll == NULL) {
    return NULL;
  }
  link_int *lll = ll->next;
  if (lll != NULL) {
    link_int *llll = lll->next;
    if (llll != NULL) {
      ll->next = llll;
    } else {
      ll->next = NULL;
    }
    return lll;
  }
  return NULL;
}

int hm_length(link* in) {
  int ret = 1;
  link* temp = in;
  while(temp->next != NULL) {
    temp = temp->next;
    ret++;
  }
  return ret;
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
