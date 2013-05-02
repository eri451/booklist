
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

/*  SAVE  {{{ */
void save_list(char* filename, tList *list)  {              /* speicher die Liste in einer Datei */

    FILE* file;
    file = fopen(filename, "wt");                           /* oeffne die Datei */

    if ( file == NULL ) {                                   /* sollte das Probleme machen */
        fprintf( stderr, "Fileerror %s\n", filename );      /* gib Bescheid und beende das Programm */
        exit(EXIT_FAILURE);
    }

    tList *savelist = NULL;
    savelist = CreateList();                                /* erzeuge eine neue Liste */
    sortList( list, savelist, BY_ARTNR, ASC_ORDER );        /* sortiere in diese ¸ber die Artikelnummer */

    lager *tmp;
    tmp = GetFirst(savelist);                                /* geh durch die sortierte Liste */
    while (tmp) {
        fprintf( file, "%i\\%s\\%i\\\n",                     /* schreibe die Werte der Datenstruktur in eine Zeile */
                 tmp->artnr,                                 /* getrennt durch "Backslash" */
                 tmp->artbez,
                 tmp->lagerbest
               );
        tmp = GetNext(savelist);
    }

    DeleteList(savelist);                                   /* loesche die nach Artikelnummer sortierte Liste */
    fclose(file);                                           /* schlieﬂe die Datei */
}

void save_element(char* filename, lager* storebook) {               /* haengt eine Datenstruktur an der Datei an */

    lager* tmp = NULL;
    FILE* file;
    tmp = malloc(sizeof(lager));                            /* lege eine neue Datenstruktur an */

    if ( tmp == NULL ) {
        fprintf( stderr, "malloc at save failed" );
        exit(EXIT_FAILURE);
    }

    file = fopen( filename, "at" );                         /* oeffne die Datei */

    if ( file == NULL ) {                                   /* sollte es Probleme geben */
        fprintf( stderr, "Fileerror %s\n\
                File can not open.\n", filename
               );                                           /* gib Bescheid und beerdige das Programm */

        exit(EXIT_FAILURE);
    }

    fprintf( file, "%i\\%s\\%i\\\n",                         /* schreibe die Werte der Datenstruktur ans Dateiende */
            tmp->artnr,                                     /* getrennt durch "Backslash" */
            tmp->artbez,
            tmp->lagerbest
           );

    fclose(file);                                           /* schlieﬂe die Datei */
}
/* }}} */

/*  LOAD  {{{ */
void load(char* filename, tList* target) {                  /* Liest eine Datei und baut daraus eine Liste */

    char vbuf[BUF_MAX];
    int j;
    char *p, *tmp;
    FILE* file;

    file = fopen( filename, "rt" );                           /* oeffne die Datei */

    if ( file == NULL ) {                                     /* sollte es Probleme geben */
        fprintf( stderr, "Fileerror %s\n\
                 File can not open.\n", filename
               );                                             /* gib Bescheid und beerdige das Programm */
        exit(EXIT_FAILURE);
    }

    while ( fgets(vbuf, BUF_MAX, file) != NULL ) {            /* solange weitere Zeilen in der Datei gefunden werden */
        lager* storebook = NULL;

        storebook = malloc(sizeof(lager));                  /* hole Speicher f‹r eine neue Datenstruktur */
        if ( storebook == NULL ) {
            fprintf( stderr, "malloc at load failed\n" );
            exit(EXIT_FAILURE);
        }

        for ( j = 0, tmp = vbuf; ; j++, tmp = NULL ) {        /* zerlege die Zeile */
            p = strtok(tmp,"\\\n");
            if ( p == NULL) {                               /* solange passende Stueckchen gefunden werden   */
                break;
            }
            switch(j) {
                case 0:     storebook->artnr = atoi(p);
                            break;
                case 2:     storebook->lagerbest = atoi(p);
                            break;
                default:    strcpy(storebook->artbez, p);   /* kopiere sie in die entsprechenden Datenfelder */
                            break;
            }
        }

        InsertSorted(   target,                             /* fuege nach Artikelnummer sortiert in die Liste ein */
                        storebook, 
                        BY_ARTNR,
                        ASC_ORDER,
                        compare 
                    );
    }
    fclose(file);                                           /* wenn alles getan ist schlieﬂe die Datei wieder */
}
/* }}} */
