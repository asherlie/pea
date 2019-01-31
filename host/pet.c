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

/* signature management */

// returns addition success
_Bool add_signature(struct petition* p, int u_id){
      for(int i = 0; i < p->n; ++i)
            if(u_id == p->signatures[i])return 0;
      if(p->n == p->cap){
            p->cap *= 2;
            int* tmp_sig = malloc(sizeof(int)*p->cap);
            memcpy(tmp_sig, p->signatures, sizeof(int)*p->n);
            free(p->signatures);
            p->signatures = tmp_sig;
      }
      p->signatures[p->n++] = u_id;
      return 1;
}

int print_sigs(FILE* fp, struct petition_container* pc){
      int total_sigs = 0;
      for(int i = 0; i < pc->n; ++i){
            total_sigs += pc->petitions[i]->n;
            fprintf(fp, "petition #%i:\n", i);
            for(int j = 0; j < pc->petitions[i]->n; ++j)
                  fprintf(fp, "  %i: %i\n", j, pc->petitions[i]->signatures[j]);
      }
      return total_sigs;
}
