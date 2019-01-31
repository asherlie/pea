struct petition{
      int n, cap;
      int* signatures;
};

struct petition_container{
      int n, cap;
      struct petition** petitions;
};

void init_pc(struct petition_container* pc);
void init_p(struct petition* p);
struct petition* alloc_p();
_Bool insert_p(struct petition* p, struct petition_container* pc);

/* signature management */

void add_signature(struct petition* p, int u_id);
