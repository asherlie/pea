#include "backup.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

// TODO: merge_pet should possibly free src
int merge_pet(struct petition_container* dest, struct petition_container* src){
      int nsigs = 0;
      struct petition* tmp_p = NULL;
      for(int i = 0; i < src->n; ++i){
            tmp_p = alloc_p();
            tmp_p->from_backup = 1;
            for(int j = 0; j < src->petitions[i]->n; ++j){
                  add_signature(tmp_p, src->petitions[i]->signatures[j], NULL);
                  ++nsigs;
            }
            insert_p(tmp_p, dest, src->petitions[i]->creator, src->petitions[i]->label);
      }
      return nsigs;
}

// returns a malloc'd petition container
struct petition_container* parse_pet(char* fpath){
      FILE* fp = fopen(fpath, "r");
      if(!fp)return NULL;

      char name[50];
      int ind = 0;
      uid_t uid;
      // a line can never be more than 100 chars long
      char ln[100]; char* ln_p = ln;
      size_t sz = 100;

      struct petition_container* pc = malloc(sizeof(struct petition_container));
      init_pc(pc);
      struct petition* p = NULL;
      while(getline(&ln_p, &sz, fp) != EOF){
            if(*ln_p == '*')continue;
            // if petition description line
            if(*ln_p != ' '){
                  p = alloc_p();
                  char* tmp_ln_p = ln_p+13;
                  char* end_paren = strrchr(tmp_ln_p, ')');
                  strncpy(name, tmp_ln_p, end_paren-tmp_ln_p);
                  sscanf(end_paren+18, "%u:", &uid);
                  insert_p(p, pc, uid, name);
            }
            else{
                  sscanf(ln_p, " %i: %u", &ind, &uid);
                  add_signature(p, uid, NULL);
            }
            memset(ln, 0, 100);
            memset(name, 0, 50);
      }
      fclose(fp);
      return pc;
}

// returns number of unique petition creators in a pc
int unique_creators(struct petition_container* pc){
      int ret = 0; 
      // +1 for spawner of daemon - getuid()
      long check[pc->n+1]; int cs = 0;
      for(int i = 0; i < pc->n; ++i)
            check[cs++] = (long)pc->petitions[i]->creator;
      check[cs] = getuid();
      for(int i = 0; i < pc->n+1; ++i)
            for(int j = 0; j < i; ++j)
                  if(check[i] == check[j])check[j] = -1;
      for(int i = 0; i < pc->n+1; ++i){
            if(check[i] != -1)++ret;
      }
      return ret;
}

// return success
_Bool setup_import_pet(struct petition_container* pc, char* fpath){
      struct petition* p = alloc_p();
      p->auto_gen = 1;
      p->restore = parse_pet(fpath);
      if(!p->restore)return 0;
      // setup p->restore->unique_creators
      p->restore->unique_creators = unique_creators(p->restore);
      printf("setting unique creators to %u\n", p->restore->unique_creators);
      if(!p->restore)return 0;
      char label[50] = {0};
      snprintf(label, 49, "RESTORE FROM %s", fpath);
      // TODO: should creator be marked ass client who entered the import command?
      insert_p(p, pc, getuid(), label);
      return 1;
}
