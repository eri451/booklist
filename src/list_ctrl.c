
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <malloc.h>

#include "list_ctrl.h"

#define MIN(a, b) ((a) < (b)) ? (a) : (b)


/*    ListControls {{{ */
int compare(int field, void* item1, void* item2) {          /* Vergleicht 2 lager-Strukturen f¸r das sortieren  */
                                                            /* ignoriert Groﬂ/Klein-Schreibung */
    switch(field) {
        case BY_ARTBEZ:
            return strcasecmp(  ( (lager*)item1)->artbez, 
                                ( (lager*)item2)->artbez
                             );
        default:
            return ( (lager*)item1)->artnr - ( (lager*)item2)->artnr;
    }
}

int match(const char* text, const char* pattern) {          /* Pattern matching fuer Sucheingabe */
                                                            /* KMP Algorithmus; ignoriert Groﬂ/Klein-Schreibung  */
    int c, d, e, text_length, pattern_length;

    text_length    = strlen(text);
    pattern_length = strlen(pattern);

    if (pattern_length > text_length) {                     /* ist der Suchbegriff l√§nger als der Text */
        return 1;                                           /* sind sie offensichtlich nicht gleich    */
    }

    for (c = 0; c <= text_length - pattern_length; c++) {
        e = c;                                              /* Positionsabgleich */

        for (d = 0; d < pattern_length; d++) {
            if (tolower(pattern[d]) == tolower(text[e])) {
                e++;                                        /* wenn pattern und text "an der selben Position" gleich: weiter machen */
            } else {
                break;                                      /* sonst Abbruch */
            }
        }

        if (d == pattern_length) {                          /* ist es geglueckt das Pattern vollstaendig durchzugehen */
            return 0;                                       /* ist das Pattern im Text enthalten */
        }
    }
    return 1;                                               /* hat das alles nicht geklappt ist das Pattern nicht enthalten */
}

void sortList(tList *from, tList *to, int by_field, int in_order) { /* sortiert eine Liste in eine andere */

    lager *tmp = NULL, *cpy = NULL;
    
    tmp = GetFirst(from);
    while (tmp != NULL) {                                     /* solange weitere Elemente in der Liste sind */
    
        cpy = malloc(sizeof(lager));
        memcpy(cpy, tmp, sizeof(lager));                      /* kopiere diese Elemente und */
    
        InsertSorted( to, cpy, by_field, in_order, compare);  /* fuege sie sortiert ein */
    
        tmp = GetNext(from);
    }
}

void sorteasyList(tList* list, int by_field, int in_order) {
    tList* sortedlist = NULL;

    sortedlist =  CreateList();

    sortList(list , sortedlist, by_field, in_order);

    /*    DeleteList(list);
          list = CreateList();
          */
    memcpy( list, sortedlist, sizeof(tList));
}

void searchList(tList *from, tList *to, const char *pattern) { /* sucht in einer Liste */

    lager *tmp = NULL, *cpy = NULL;

    tmp = GetFirst(from);
    while (tmp != NULL) {                                   /* solange weitere Elemente in Liste sind */
    
        if (match(tmp->artbez,   pattern) == 0               /* falls das Pattern in title oder debitor ist */
         || match((char*)tmp->artnr, pattern) == 0
         || match((char*)tmp->lagerbest, pattern) == 0) {

            cpy = malloc(sizeof(lager));                    /* kopiere das Element und sortiere es in eine neue Liste ein */
            memcpy(cpy, tmp, sizeof(lager));
            InsertSorted(to, cpy, BY_ARTBEZ, ASC_ORDER, compare);
        }
        tmp = GetNext(from);
    }
}

int get_new_artnr(tList *list) {                            /* erzeugt eine neue Artikelnummer */
    int new_artnr = 0;
    lager* tmp = NULL;

    tmp = GetFirst(list);
    while(tmp != NULL) {                                    /* gehe durch die Liste bis du alle Elemente untersucht hast */
    
        if (tmp->artnr > new_artnr) {                       /* falls die Artikelnummer des gerade betrachteten Elements groesser ist */
            new_artnr = tmp->artnr;                         /* als die groesse der bisher betrachteten Elemente */
        }
        tmp = GetNext(list);                                /* merke diese Artikelnummer */
    }
    return ++new_artnr;                                     /* lege noch einen drauf; et voila neue Artikelnummer */
}

void add_to_list(     tList*    list,                       /* fuege ein neues Element der Liste hinzu */
                 const  int*    artnr,
                 const char*    artbez,
                 const  int*    lagerbest
                ) {

    lager *tmp = NULL;
    int len = 0;

    tmp = malloc(sizeof(lager));                            /* lege eine neue Datenstruktur an */
    if (tmp) {
        len = MIN(sizeof(tmp->artbez), strlen(artbez));     /* uebertrage die Werte */
        strncpy(tmp->artbez, artbez, len);
        tmp->artbez[len]   = 0;

        tmp->lagerbest = *lagerbest;

        tmp->artnr = *artnr;

//        save(filename, tmp);                                   /* Speicher das Element auf der Festplatte */
        InsertSorted(list, tmp, BY_ARTNR, ASC_ORDER, compare); /* fuege des sortiert ein */
    }
}

void remove_from_list(tList *list, int artnr_ident) {       /* Loescht einen Listeneintrag mit der Artikelnummer ident */

    lager *tmp = NULL;
    
    tmp = GetFirst(list);

    while (tmp) {                                           /* Lauf durch die Liste */
        if (tmp->artnr == artnr_ident) {                    /* wenn ein Eintrag die Artikelnummer ident hat */
            RemoveItem(list);                               /* loesche diesen */
            break;
        }
        tmp = GetNext(list);
    }
//    save_list(filename, list);                              /* speicher die Liste auf der Festplatte */
}
/* }}} */

/* DISPLAY {{{ */
void print_all_list(tList* list) {
    
    lager *tmp = NULL;
    
    tmp = GetFirst(list);

    while (tmp) {                                           /* Lauf durch die Liste */
        printf("\t\tArtikelnummer:\t\t%d\n\
                Artikelbezeichnung:\t%s\n\
                Lagerbestand:\t\t%d\n\n", tmp->artnr, tmp->artbez, tmp->lagerbest );
        
        tmp = GetNext(list);
    }

}
/* }}} */
