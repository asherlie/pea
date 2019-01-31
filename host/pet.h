/*
 * petitions are unlabeled
 * a list of labeled petitions should be distrubuted
 * separately in a mailing list
 * petitions are referred to within the PEA ecosystem 
 * by their index within the petition_container
 */

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

_Bool add_signature(struct petition* p, int u_id);
