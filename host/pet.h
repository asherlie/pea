#include <stdio.h>
#include <sys/types.h>

/* struct definitions */

struct petition{
      char label[50];
      int n, cap;
      uid_t creator;
      uid_t* signatures;
      _Bool from_backup, auto_gen;

      /* alt_count is used to count relevant signatures of unique_creators
       * to import petition files
       */
      struct petition_container* restore;
      int alt_count;
};

struct petition_container{
      int unique_creators;
      int n, cap;
      struct petition** petitions;
};

/* petition structure management */

void init_pc(struct petition_container* pc);
void init_p(struct petition* p);
struct petition* alloc_p();
_Bool insert_p(struct petition* p, struct petition_container* pc, uid_t creator, char* label);
_Bool remove_p(struct petition_container* pc, int index);
_Bool remove_sig(struct petition* p, uid_t uid);

/* signature management */

_Bool add_signature(struct petition* p, uid_t u_id, _Bool* added);
int print_sigs(FILE* fp, struct petition_container* pc);
