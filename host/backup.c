#include "backup.h"

#include <stdio.h>
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

int apply_backup(struct petition_container* pc, char* backup_file){
      struct petition_container* tmp_pc = parse_pet(backup_file);
      int ret = merge_pet(pc, tmp_pc);
      for(; tmp_pc->n; remove_p(tmp_pc, 0));
      free(tmp_pc->petitions);
      free(tmp_pc);
      return ret;
}
