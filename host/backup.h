#include "pet.h"

int merge_pet(struct petition_container* dest, struct petition_container* src);
struct petition_container* parse_pet(char* fpath);
int apply_backup(struct petition_container* pc, char* backup_file);
