
#ifndef __LIST_CTRL_H
#define __LIST_CTRL_H

#include "list.h"

#define BUF_MAX 256

enum sort {
    BY_ARTNR,           /* chronological */
    BY_ARTBEZ
};

typedef struct {     /* lager Datenstuktur */
    int  artnr;
    char artbez[BUF_MAX];
    int  lagerbest;
} lager;

int compare(int field, void* item1, void* item2);

int match(const char* text, const char* pattern);

void sortList(tList* from, tList* to, int by_field, int in_order);

void sorteasyList(tList* list, int by_field, int in_order);

void searchList(tList* from, tList* to, const char* pattern);

int get_new_artnr(tList* list);

void add_to_list(     tList*    list,
                 const  int*    artnr,
                 const char*    artbez,
                 const  int*    lagerbest
                );

void remove_from_list(tList* list, int artnr);

void print_all_list(tList* list); 

#endif // __LIST_CTRL_H
