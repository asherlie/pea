#include "backup.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

/*
 * TODO:
 * create backups every 20 seconds if entries have changed
 * with permissions 0444 - pea_h will either be started with an optional arg
 * for a file backup or this will be suggested by a user with import clientsi
 * de command with a filepath to the backup file
 *
 * this will create a petition that is marked off (automatically generated pet
 * ition). once signed by all creators of petitions in backup file, this pet
 * will be deleted or marked as complete. each entry will say # of total requi
 * red to merge the petition file with the current pc*
 *
 */

// TODO: merge_pet should possibly free src
int merge_pet(struct petition_container* dest, struct petition_container* src){
      int nsigs = 0;
      struct petition* tmp_p = NULL;
      for(int i = 0; i < src->n; ++i){
            tmp_p = alloc_p();
            tmp_p->from_backup = 1;
            for(int j = 0; j < src->petitions[i]->n; ++j){
                  add_signature(tmp_p, src->petitions[i]->signatures[j]);
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
      char* name_p;
      int ind = 0;
      uid_t uid;
      // a line can never be more than 100 chars long
      char ln[100]; char* ln_p = ln;
      size_t sz = 100;

      struct petition_container* pc = malloc(sizeof(struct petition_container));
      init_pc(pc);
      struct petition* p = NULL;
      while(getline(&ln_p, &sz, fp) != EOF){
            if(*ln_p == '~')continue;
            // if petition description line
            if(*ln_p != ' '){
                  p = alloc_p();
                  char* tmp_ln_p = ln_p+13;
                  sscanf(tmp_ln_p, "%s) ", name);
                  name_p = name;
                  int len = strnlen(name_p, 48);
                  name_p[len-1] = 0;
                  tmp_ln_p += len+17;
                  sscanf(tmp_ln_p, "%u:", &uid);
                  insert_p(p, pc, uid, name);
            }
            else{
                  sscanf(ln_p, " %i: %u", &ind, &uid);
                  add_signature(p, uid);
            }
            memset(ln, 0, 100);
            memset(name, 0, 50);
      }
      fclose(fp);
      return pc;
}

int unique_creators(struct petition_container* pc){
}

// return success
_Bool setup_import_pet(struct petition_container* pc, char* fpath){
      struct petition* p = alloc_p();
      p->auto_gen = 1;
      p->restore = parse_pet(fpath);
      // setup p->restore->unique_creators
      if(!p->restore)return 0;
      char label[50] = {0};
      snprintf(label, 49, "RESTORE FROM %s", fpath);
      insert_p(p, pc, getuid(), label);
      return 1;
}
