#include "pet.h"
#include <stdlib.h>
#include <string.h> // memcpy

void init_pc(struct petition_container* pc){
      pc->n = 0;
      pc->cap = 10;
      pc->petitions = malloc(sizeof(struct petition)*pc->cap);
}

void init_p(struct petition* p){
      memset(p->label, 0, 50);
      p->creator = -1;
      p->n = 0;
      p->cap = 10;
      p->signatures = malloc(sizeof(uid_t)*p->cap);
}

struct petition* alloc_p(){
      struct petition* pet = malloc(sizeof(struct petition));
      init_p(pet);
      return pet;
}

// inserts petition into petition container
// returns whether or not pc has been resized
_Bool insert_p(struct petition* p, struct petition_container* pc, uid_t creator, char* label){
      _Bool ret;
      if((ret = pc->n == pc->cap)){
            pc->cap *= 2;
            struct petition** tmp_pet = malloc(sizeof(struct petition*)*pc->cap);
            memcpy(tmp_pet, pc->petitions, sizeof(struct petition*)*pc->n);
            free(pc->petitions);
            pc->petitions = tmp_pet;
      }
      strncpy(p->label, label, 49);
      p->creator = creator;
      pc->petitions[pc->n++] = p;
      return ret;
}

_Bool remove_p(struct petition_container* pc, int index){
      /* 
       * not bothering with reallocating when we could just shift the
       * latter part of the struct petition** left
       */
      // memmove for overlapping mem
      free(pc->petitions[index]->signatures);
      pc->petitions[index]->signatures = NULL;
      free(pc->petitions[index]);
      pc->petitions[index] = NULL;
      memmove(&pc->petitions[index], &pc->petitions[index+1], sizeof(struct petition*)*((pc->n--)-index-1));
      return 1;
}

_Bool remove_sig(struct petition* p, uid_t uid){
      int i;
      for(i = 0; i < p->n; ++i)
            if(p->signatures[i] == uid){
            }
      return 0;
}

/* signature management */

// returns addition success
_Bool add_signature(struct petition* p, uid_t u_id){
      for(int i = 0; i < p->n; ++i)
            if(u_id == p->signatures[i])return 0;
      if(p->n == p->cap){
            p->cap *= 2;
            uid_t* tmp_sig = malloc(sizeof(uid_t)*p->cap);
            memcpy(tmp_sig, p->signatures, sizeof(uid_t)*p->n);
            free(p->signatures);
            p->signatures = tmp_sig;
      }
      p->signatures[p->n++] = u_id;
      return 1;
}

// if fp is anything but stdout, it must be closed after print_sigs is called
int print_sigs(FILE* fp, struct petition_container* pc){
      int total_sigs = 0;
      for(int i = 0; i < pc->n; ++i){
            total_sigs += pc->petitions[i]->n;
            fprintf(fp, "petition #%i (%s) created by user %i:\n", i, pc->petitions[i]->label, pc->petitions[i]->creator);
            for(int j = 0; j < pc->petitions[i]->n; ++j)
                  fprintf(fp, "  %i: %i\n", j, pc->petitions[i]->signatures[j]);
      }
      return total_sigs;
}
