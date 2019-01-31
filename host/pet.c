#include "pet.h"
#include <stdlib.h>

void init_pc(struct petition_container* pc){
      pc->n = 0;
      pc->cap = 10;
      pc->petitions = malloc(sizeof(struct petition)*pc->cap);
}

void init_p(struct petition* p){
      p->n = 0;
      p->cap = 10;
      p->signatures = malloc(sizeof(int)*p->cap);
}

struct petition* alloc_p(){
      struct petition* pet = malloc(sizeof(struct petition));
      init_p(pet);
      return pet;
}
