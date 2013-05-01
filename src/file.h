
#ifndef __FILE_H
#define __FILE_H
#include "list_ctrl.h"


void save_list(char* filename, tList* list);

void save(char* filename, lager* storebook);

void load(char* filename, tList* target);

#endif // __FILE_H
