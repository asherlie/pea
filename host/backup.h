#include "pet.h"

int merge_pet(struct petition_container* dest, struct petition_container* src);
struct petition_container* parse_pet(char* fpath);
_Bool setup_import_pet(struct petition_container* pc, char* fpath);
