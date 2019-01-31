struct petition{
      int n, cap;
      int* signatures;
};

struct petition_container{
      int n, cap;
      struct petition* petitions;
};

void init_pc(struct petition_container* pc);
void init_p(struct petition* p);
struct petition* alloc_p();
