/*****************************************************************************
 * ctrl.c                                                                    *
 * Copyright (C) 2013  eri! <hans.orter@gmx.de>                              *
 *                                                                           *
 * This file is part of Booklist.                                            *
 *                                                                           *
 * Booklist is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation, either version 3 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * Foobar is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with Booklist.  If not, see <http://www.gnu.org/licenses/>.         *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "ctrl.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define FILE_PATH "./data/booklist.txt"
char* filename = FILE_PATH;

/*    ListControls {{{ */
int compare(int field, void* item1, void* item2)   /* Vergleicht 2 media-Strukturen   */
{                                                  /* ignoriert Groß/Klein-Schreibung */
    switch(field){
        case BY_TITLE:
            return strcasecmp(((media*)item1)->title  , ((media*)item2)->title);
        case BY_AUTHOR:
            return strcasecmp(((media*)item1)->author , ((media*)item2)->author);
        case BY_DEBITOR:
            return strcasecmp(((media*)item1)->debitor, ((media*)item2)->debitor);
        default:
            return ((media*)item1)->id - ((media*)item2)->id;
    }
}

int match(const char* text, const char* pattern)    /* Pattern matching fuer Sucheingabe */
{                                    /* KMP Algorithmus; ignoriert Groß/Klein-Schreibung  */
    int c, d, e, text_length, pattern_length;
    text_length    = strlen(text);
    pattern_length = strlen(pattern);

    if (pattern_length > text_length)    /* ist der Suchbegriff länger als der Text */
        return 1;                        /* sind sie offensichtlich nicht gleich    */

    for (c = 0; c <= text_length - pattern_length; c++)
    {
        e = c;                           /* Positionsabgleich */
        for (d = 0; d < pattern_length; d++)
        {
            if (tolower(pattern[d]) == tolower(text[e]))
                e++;                     /* wenn pattern und text "an der selben Position" gleich: weiter machen */
            else
                break;                   /* sonst Abbruch */
        }
        if (d == pattern_length)         /* ist es geglueckt das Pattern vollstaendig durchzugehen */
            return 0;                    /* ist das Pattern im Text enthalten */
    }
    return 1;                            /* hat das alles nicht geklappt ist das Pattern nicht enthalten */
}

void sortList(tList *from, tList *to, int by_field, int in_order) /* sortiert eine Liste in eine andere */
{
    media *tmp = NULL, *cpy = NULL;
    tmp = GetFirst(from);
    while (tmp != NULL)   /* solange weitere Elemente in der Liste sind */
    {
        cpy = malloc(sizeof(media));
        memcpy(cpy, tmp, sizeof(media));                      /* kopiere diese Elemente und */
        InsertSorted( to, cpy, by_field, in_order, compare);  /* fuege sie sortiert ein */
        tmp = GetNext(from);
    }
}

void seachList(tList *from, tList *to, const char *pattern) /* sucht in einer Liste */
{
    media *tmp = NULL, *cpy = NULL;
    tmp = GetFirst(from);
    while (tmp != NULL) /* solange weitere Elemente in Liste sind */
    {
        if (match(tmp->title,   pattern) == 0    /* falls das Pattern in title oder debitor ist */
         || match(tmp->debitor, pattern) == 0)
        {
            cpy = malloc(sizeof(media));    /* kopiere das Element und sortiere es in eine neue Liste ein */
            memcpy(cpy, tmp, sizeof(media));
            InsertSorted(to, cpy, BY_TITLE, ASC_ORDER, compare);
        }
        tmp = GetNext(from);
    }
}

int get_new_id(tList *list)  /* erzeugt eine neue ID */
{
    int identi = 0;
    media* tmp = NULL;
    tmp = GetFirst(list);
    while(tmp != NULL)   /* gehe durch die Liste bis du alle Elemente untersucht hast */
    {
        if (tmp->id > identi)  /* falls die ID des gerade betrachteten Elements groesser ist */
            identi = tmp->id;  /* als die groesse der bisher betrachteten Elemente */
        tmp = GetNext(list);   /* merke diese ID */
    }
    return ++identi;           /* lege noch einen drauf; et voila neue ID */
}

void add_to_list(tList *list,               /* fuege ein neues Element der Liste hinzu */
                  const char* title,
                  const char* author,
                  const char* debitor,
                  const char* date,
                         int  ident)
{
    media *tmp = NULL;
    int len = 0;

    tmp = malloc(sizeof(media));   /* lege eine neue Datenstruktur an */
    if (tmp){
        len = MIN(sizeof(tmp->title),   strlen(title));   /* uebertrage die Werte */
        strncpy(tmp->title,   title ,   len);
        tmp->title[len]   = 0;

        len = MIN(sizeof(tmp->author),  strlen(author));
        strncpy(tmp->author,  author ,  len);
        tmp->author[len]  = 0;

        len = MIN(sizeof(tmp->debitor), strlen(debitor));
        strncpy(tmp->debitor, debitor , len);
        tmp->debitor[len] = 0;

        len = MIN(sizeof(tmp->date),    strlen(date));
        strncpy(tmp->date, date, len);
        tmp->date[len] = 0;

        tmp->id = ident;

        save(filename, tmp);     /* Speicher das Element auf der Festplatte */
        InsertSorted(list, tmp, BY_ID, ASC_ORDER, compare); /* fuege des sortiert ein */
    }
}

void remove_from_list(tList *list, int ident) /* Loescht einen Listeneintrag mit der ID ident */
{
    media *tmp = NULL;
    tmp = GetFirst(list);

    while (tmp){    /* Lauf durch die Liste */
        if (tmp->id == ident){  /* wenn ein Eintrag die ID ident hat */
            RemoveItem(list);   /* loesche diesen */
            break;
        }
        tmp = GetNext(list);
    }
    save_list(filename, list); /* speicher die Liste auf der Festplatte */
}
/* }}} */

/*  SAVE  {{{ */
void save_list(char* filename, tList *list)  /* speicher die Liste in einer Datei */
{
    FILE* file;
    file = fopen(filename, "wt");  /* oeffne die Datei */

    if (file == NULL){ /* sollte das Probleme machen */
        fprintf(stderr, "Fileerror %s\n", filename); /* gib Bescheid und beende das Programm */
        exit(EXIT_FAILURE);
    }

    tList *savelist = NULL;
    savelist = CreateList();                    /* erzeuge eine neue Liste */
    sortList(list, savelist, BY_ID, ASC_ORDER); /* sortiere in diese über die ID */

    media *tmp;
    tmp = GetFirst(savelist);       /* geh durch die sortierte Liste */
    while (tmp){
        fprintf(file, "%s\\%s\\%s\\%s\n",   /* schreibe die Werte der Datenstruktur in eine Zeile */
                tmp->date,                  /* getrennt durch "Backslash" */
                tmp->title,
                tmp->debitor,
                tmp->author);
        tmp = GetNext(savelist);
    }

    DeleteList(savelist);          /* loesche die nach ID sortierte Liste */
    fclose(file);                  /* schließe die Datei */
}

void save(char* filename, media* localbook)  /* haengt eine Datenstruktur an der Datei an */
{
    FILE* file;
    file = fopen(filename, "at");  /* oeffne die Datei */

    if (file == NULL){                               /* sollte es Probleme geben */
        fprintf(stderr, "Fileerror %s\n", filename); /* gib Bescheid und beende das Programm */
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%s\\%s\\%s\\%s\n",  /* schreibe die Werte der Datenstruktur ans Dateiende */
            localbook->date,
            localbook->title,
            localbook->debitor,
            localbook->author);

    fclose(file);                 /* schließe die Datei */
}
/* }}} */

/*  LOAD  {{{ */
void load(char* filename, tList* target)  /* Liest eine Datei und baut daraus eine Liste */
{
    char vbuf[BUF_MAX];

    FILE* file;                   /* oeffne die Datei */
    file = fopen(filename, "rt");

    if (file == NULL){                               /* sollte es Probleme geben */
        fprintf(stderr, "Fileerror %s\n", filename); /*gib Bescheid und beende das Programm */
        exit(EXIT_FAILURE);
    }

    int j;
    int i = 0;  /* beginne mit den IDs bei 0 */
    char *p, *tmp;

    while (fgets(vbuf, BUF_MAX, file) != NULL){   /* solange weitere Zeilen in der Datei gefunden werden */
        media* localbook = NULL;

        localbook = malloc(sizeof(media));        /* hole Speicher für eine neue Datenstruktur */
        if (localbook == NULL){
            fprintf(stderr, "malloc at load failed\n");
            exit(EXIT_FAILURE);
        }

        char* items[4] = {
                          localbook->date,
                          localbook->title,
                          localbook->debitor,
                          localbook->author
                         };
        for (j = 0, tmp = vbuf; ; j++, tmp = NULL){  /* zerlege die Zeile */
            p = strtok(tmp,"\\\n");
            if ( p == NULL)                          /* solange passende Stueckchen gefunden werden   */
                break;
            strcpy(items[j], p);                     /* kopiere sie in die entsprechenden Datenfelder */
        }
        localbook->id = i;                           /* schreib eine ID dazu */

        InsertSorted(target, localbook, BY_ID, ASC_ORDER, compare); /* fuege nach ID sortiert in die Liste ein */
        i++;                                         /* zaehle die ID eins weiter */
    }
    fclose(file);  /* wenn alles getan ist schließe die Datei wieder */
}
/* }}} */

int main(int argc, char *argv[])  /* Programmstart */
{
    tList *medialist = NULL;

    if (argc == 2)
    {
        filename = (char*)calloc(strlen(argv[1]), sizeof(char));
        strncpy(filename,argv[1],strlen(argv[1]));
    }

    media *book = NULL;
    medialist = CreateList();    /* erzeuge eine neue Liste       */
    load(filename, medialist);  /* Lade die Datei in diese Liste */

    gtk_init(&argc, &argv);      /* initialisiere die graphische Nutzerschnittstelle */
    draw_gui(medialist);         /* stell die Liste dem Nutzer dar */
    gtk_main();                  /* uebergib GTK seine Rechte und Pflichten */

    if (strcmp(filename, FILE_PATH))
        free(filename);

    return EXIT_SUCCESS;         /* das war's */
}

/* vim: set et sw=4 st=4: */
