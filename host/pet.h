#include <stdio.h>

struct petition{
      char label[50];
      int n, cap, creator;
      int* signatures;
};

struct petition_container{
      int n, cap;
      struct petition** petitions;
};

void init_pc(struct petition_container* pc);
void init_p(struct petition* p);
struct petition* alloc_p();
_Bool insert_p(struct petition* p, struct petition_container* pc, int creator, char* label);

/* signature management */

_Bool add_signature(struct petition* p, int u_id);
int print_sigs(FILE* fp, struct petition_container* pc);
