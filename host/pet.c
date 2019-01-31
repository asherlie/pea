#include "pet.h"
#include <stdlib.h>
#include <string.h> // memcpy

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

// inserts petition into petition container
// returns whether or not pc has been resized
_Bool insert_p(struct petition* p, struct petition_container* pc){
      _Bool ret;
      if((ret = pc->n == pc->cap)){
            pc->cap *= 2;
            struct petition** tmp_pet = malloc(sizeof(struct petition*)*pc->cap);
            memcpy(tmp_pet, pc->petitions, sizeof(struct petition*)*pc->n);
            free(pc->petitions);
            pc->petitions = tmp_pet;
      }
      pc->petitions[pc->n++] = p;
      return ret;
}
