#ifndef __FILE_H
#define __FILE_H

#include "list.h"

#define BUF_MAX 256

enum sort
{
    BY_ID,           /* chronological */
    BY_TITLE,
    BY_AUTHOR,
    BY_DEBITOR
};

typedef struct{     /* media Datenstuktur */
    int  id;
    char date[BUF_MAX];
    char title[BUF_MAX];
    char author[BUF_MAX];
    char debitor[BUF_MAX];
} media;

int compare(int field, void* item1, void* item2);

int match(const char* text, const char* pattern);

void sortList(tList *from, tList *to, int by_field, int in_order);

void sortTheList(tList* list, int by_field, int in_order);    /* um die selbe list sortiert zurück zu geben */

void seachList(tList *from, tList *to, const char *pattern);

int get_new_id(tList *list);

void add_to_list(tList *list,
                 const char* title,
                 const char* author,
                 const char* debitor,
                 const char* date,
                       int   ident);

void remove_from_list(tList *list, int ident);

void save_list(char* filename, tList *list);

void save(char* filename, media* localbook);

void load(char *filename, tList *target);

#endif // __FILE_H

/* vim: set et sw=4 ts=4 */
