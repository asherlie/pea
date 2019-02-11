#include "pet.h"
#include <stdlib.h>
#include <unistd.h> // getuid
#include <string.h> // memcpy

void init_pc(struct petition_container* pc){
      pc->unique_creators = 0;
      pc->n = 0;
      pc->cap = 10;
      pc->petitions = malloc(sizeof(struct petition)*pc->cap);
}

void init_p(struct petition* p){
      memset(p->label, 0, 50);
      p->creator = -1;
      p->from_backup = p->auto_gen = 0;
      p->n = p->alt_count = 0;
      p->cap = 10;
      p->restore = NULL;
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

// TODO: UB on rm pet[0] because a read is made to pet[1] - handle n == 0
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

// TODO: UB on rm sig[0] because a read is made to sig[1] - handle n == 0
_Bool remove_sig(struct petition* p, uid_t uid){
      for(int i = 0; i < p->n; ++i)
            if(p->signatures[i] == uid){
                  memmove(&p->signatures[i], &p->signatures[i+1], sizeof(int)*((p->n--)-i-1));
                  return 1;
            }
      return 0;
}

/* signature management */

// returns auto-generated signature completion
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
      if(p->auto_gen && p->restore){
            // these cases can be handled separately because of the
            // duplicate checking above
            if(u_id == getuid())++p->alt_count;
            else{
                  for(int i = 0; i < p->restore->n; ++i){
                        if(u_id == p->restore->petitions[i]->creator){
                              ++p->alt_count; break;
                        }
                  }
            }
      }
      return (p->auto_gen && p->restore) ? p->alt_count == p->restore->unique_creators : 0;
}

// if fp is anything but stdout, it must be closed after print_sigs is called
int print_sigs(FILE* fp, struct petition_container* pc){
      int total_sigs = 0;
      for(int i = 0; i < pc->n; ++i){
            total_sigs += pc->petitions[i]->n;
            if(pc->petitions[i]->auto_gen){
                  fputs("** this petition has been automatically generated by PEA **\n", fp);
                  if(pc->petitions[i]->restore){
                        fprintf(fp, "** file backup will be restored once the %i necessary signatures are collected **\n", pc->petitions[i]->restore->unique_creators);
                        fprintf(fp, "** %i of %i required signatures have been collected  **\n", pc->petitions[i]->alt_count, pc->petitions[i]->restore->unique_creators);
                  }
                  else fputs("** file backup has been completed **\n", fp);
            }
            if(pc->petitions[i]->from_backup)
                  fputs("** this petition has been restored from a backup **\n", fp);
            fprintf(fp, "petition #%i (%s) created by user %i:\n", i, pc->petitions[i]->label, pc->petitions[i]->creator);
            for(int j = 0; j < pc->petitions[i]->n; ++j)
                  fprintf(fp, "  %i: %i\n", j, pc->petitions[i]->signatures[j]);
      }
      return total_sigs;
}
