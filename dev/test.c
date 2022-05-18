#include <assert.h>


int main() {

  int a = 33;
  int *p = &a;
  int *d = &a;

  assert(d != p);
}
