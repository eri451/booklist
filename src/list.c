/*****************************************************************************
 * list.c                                                                    *
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

#include <stdlib.h>
#include <malloc.h>
#include "list.h"

/*    Listenops   {{{*/

tList *CreateList(void)  /* erzeugt eine Liste */
{
    tList *ptmp = NULL;              /* erzeuge einen Zeiger fuer eine neue Listenstruktur */
    ptmp = malloc(sizeof (tList));   /* nimm soviel Speicher wie eine Listen-Struktur braucht */
    if (ptmp)                        /* und gib ptmp die Adresse. Falls das glueckte */
        ptmp->phead = ptmp->ptail = ptmp->pcurr = NULL;   /* lass alle Zeiger der Struktur auf 0x0 zeigen */
    return ptmp;                    /* dann gib mir den Zeiger auf meine neue Listenstruktur */
}

long DeleteList(tList *pList)  /* Loescht eine Liste und ihre Elemente */
{
    if (pList == NULL)   /* falls dir ein Verweis auf 0x0 gegeben wird mache gar nichts */
        return 0;

    tcnct *tmp;
    tmp = GetFirst(pList);   /* nimm das erste Listenelement der Liste */
    while (tmp != NULL)      /* solange du weitere Elemente findest */
    {
        RemoveItem(pList);   /* schmeiss sie aus dieser Liste raus */
        tmp = GetNext(pList);
    }
    pList->phead = pList->ptail = pList->pcurr = NULL;
    free(pList);             /* gib den belegten Speicher wieder frei */
    return 0;
}

/*}}}*/
/*     INSERTS     {{{*/

long InsertHead(tList *pList, void *pItemIns)   /* Fuege ein Element am Anfang ein */
{
    tcnct *ptmp = NULL;                     /* erzeuge einen Zeiger auf ein neues Element */
    ptmp = malloc(sizeof(tcnct));           /* hol Speicher fuer ein neues Listenelement */
    if(ptmp)                                /* geglueckt? */
    {
        ptmp->pdata = ptmp->pnext = ptmp->pprev = NULL;
        ptmp->pdata = pItemIns;            /* der pdata-Zeiger soll ab jetzt auf die Datenstruktur zeigen */
        ptmp->pnext = pList->phead;        /* Sei das naechste Element das "alte" Anfangselement */
                                           /* Damit ist das neue Element davor (also am Anfang) */
        if (pList->ptail == NULL)          /* gibt es kein Ende sei ptmp das Element am Ende */
            pList->ptail = ptmp;
        if (pList->phead != NULL)          /* Sollte es schon ein Anfangselement geben soll dessen Vorgaenger */
            pList->phead->pprev = ptmp;    /* dieses Element sein */

        pList->phead = ptmp;               /* das Anfangselement der Liste sei das soeben eingefuegte */
        pList->pcurr = ptmp;               /* das aktuelle Element ist das soeben eingefuegte*/
    }
    return (long)ptmp;                     /* gib die Adresse des aktuellen Elements zurueck */
}

long InsertTail(tList *pList, void *pItemIns) /* Fuege ein Element am Ende der Liste ein */
{                                             /* Prinzip analog zu InsertHead */
    tcnct *ptmp = NULL;
    ptmp = malloc(sizeof(tcnct));
    if(ptmp)
    {
        ptmp->pdata = ptmp->pnext = ptmp->pprev = NULL;
        ptmp->pdata = pItemIns;
        ptmp->pprev = pList->ptail;

        if (pList->phead == NULL)
            pList->phead = ptmp;
        if (pList->ptail != NULL)
            pList->ptail->pnext = ptmp;

        pList->ptail = ptmp;
        pList->pcurr = ptmp;
    }
    return (long)ptmp;
}

long InsertBehind(tList *pList, void *pItemIns) /* Fuege ein Element hinter dem aktuellen ein */
{
    if ((pList->phead == NULL && pList->ptail == NULL) || pList->pcurr == NULL)
        return InsertTail(pList, pItemIns);     /* falls die Liste keinen Anfang und kein Ende hat oder das */
                                                /* aktuelle Element nicht vorhanden ist fuege am Ende ein    */
    tcnct *ptmp = NULL;
    ptmp = malloc(sizeof(tcnct));               /* hol Speicher fuer ein neues Element */
    if(ptmp)
    {
        ptmp->pdata = ptmp->pnext = ptmp->pprev = NULL;
        ptmp->pdata = pItemIns;                 /* der pdata-Zeiger soll ab jetzt auf die Datenstruktur zeigen */
        ptmp->pprev = pList->pcurr;             /* der Vorgaenger des neuen Elements sei das aktuelle */
        ptmp->pnext = pList->pcurr->pnext;      /* der Nachfolger sei der Nachfolger des aktuellen Elements*/

        if (pList->pcurr->pnext != NULL)        /* falls das aktuelle Element keinen Nachfolger hat*/
            pList->pcurr->pnext->pprev = ptmp;  /* soll der Vorgaenger des Nachfolgers des aktuellen Elements das neue sein */
        if (pList->pcurr == pList->ptail)       /* falls das das aktuelle Element das Element am Ende der Liste ist*/
            pList->ptail  = ptmp;               /* dann sei das Element am Ende des Liste das neue Element */

        pList->pcurr->pnext = ptmp;             /* der Nachfolger des aktuellen Elements sei das neue Element */
        pList->pcurr = ptmp;                    /* das aktuelle Element sei das neue Element */
    }
    return (long)ptmp;                          /* gib die Adresse des aktuellen Elements zurueck */
}

long InsertBefore(tList *pList, void *pItemIns) /* Fuege ein Element vor dem aktuellen ein*/
{                                               /* analog zu InsertBehind */
    if ((pList->phead == NULL && pList->ptail == NULL) || pList->pcurr == NULL)
        return InsertHead(pList, pItemIns);

    tcnct *ptmp = NULL;
    ptmp = malloc(sizeof(tcnct));
    if (ptmp)
    {
        ptmp->pdata = ptmp->pnext = ptmp->pprev = NULL;
        ptmp->pdata = pItemIns;
        ptmp->pnext = pList->pcurr;
        ptmp->pprev = pList->pcurr->pprev;

        if (pList->pcurr->pprev != NULL)
            pList->pcurr->pprev->pnext = ptmp;
        if (pList->pcurr == pList->phead)
            pList->phead  = ptmp;

        pList->pcurr->pprev = ptmp;
        pList->pcurr = ptmp;
    }
    return (long)ptmp;
}

long InsertSorted(tList *pList,           /* Fuege sortiert in eine Liste ein */
                  void  *pItemIns,
                  int    field,
                  int    order,
                  int  (*compare)(int, void*, void*)
                 )
{
    if (pList->pcurr == NULL)                    /* ist die Liste leer */
        return InsertHead(pList, pItemIns);      /* fuege am Anfang ein */

    void *tmp = NULL;
    if (order == ASC_ORDER)                      /* falls aufsteigend  */
    {
        tmp = GetFirst(pList);                   /* nimm das erste Element der Liste */
        while (tmp != NULL && compare(field, tmp, pItemIns) < 0)   /* solange das schon enthaltene Element "kleiner" ist */
            tmp = GetNext(pList);                                  /* vergleiche mit dem naechsten */
        if (tmp != NULL)                         /* stoesst du auf eines das "groesser" ist */
            InsertBefore(pList, pItemIns);       /* fuege das neue Element davor ein */
        else
            InsertTail(pList, pItemIns);         /* waren alle "kleiner" fuege das neue Element am Ende ein */
    }
    else
    {
        tmp = GetLast(pList);                    /* falls absteigend: invertieres Einfuegen */
        while (tmp != NULL && compare(field, tmp, pItemIns) < 0)
            tmp = GetPrev(pList);
        if (tmp != NULL)
            InsertBehind(pList, pItemIns);
        else
            InsertHead(pList, pItemIns);
    }
    return (long)pList->pcurr;
}

/*}}}*/
/*     GETS        {{{*/

void *GetSelected(tList *pList)
{  /* hol vom aktuellen Element der Liste die Adresse der Datenstruktur und gib diese zurueck */
    void *ptmp = NULL;
    if(pList->pcurr)
    {
        ptmp = pList->pcurr->pdata;
    }
    return ptmp;
}

void *GetFirst(tList *pList)
{  /* hol vom Anfangselement der Liste die Adresse der Datenstruktur und gib diese zurueck */
    void *ptmp = NULL;
    if(pList->phead)
    {
        pList->pcurr = pList->phead;   /* setzt das Element am Ende der List als aktuelles Element */
        ptmp = pList->pcurr->pdata;    /* gib ptmp die Adresse der Datenstruktur */
    }
    return ptmp;
}

void *GetLast(tList *pList)
{  /* hol vom Element am Ende der Liste die Adresse der Datenstruktur und gib diese zurueck */
    void *ptmp = NULL;
    if(pList->ptail)
    {
        pList->pcurr = pList->ptail;  /* setzt das Element am Anfang der Liste als aktuelles Element */
        ptmp = pList->pcurr->pdata;   /* gib ptmp die Adresse der Datenstruktur */
    }
    return ptmp;
}

void *GetNext(tList *pList)
{  /* hol vom Nachfolger des aktuellen Elements die Adresse der Datenstruktur und gib diese zurueck */
    void *ptmp = NULL;
    if(pList->pcurr && pList->pcurr->pnext)  /* sofern es ein aktuelles Element und davon einen Nachfolger gibt */
    {
        pList->pcurr = pList->pcurr->pnext;  /* mache den Nachfolger zum aktuellen Element */
        ptmp = pList->pcurr->pdata;          /* gib ptmp die Adresse der Datenstruktur */
    }
    return ptmp;
}

void *GetPrev(tList *pList)
{  /* hol vom Vorgaenger des aktuellen Elements die Adresse der Datenstruktur und gib diese zurueck */
    void *ptmp = NULL;
    if(pList->pcurr && pList->pcurr->pprev)  /* sofern es ein aktuelles Element und davon einen Vorgaenger gibt */
    {
        pList->pcurr = pList->pcurr->pprev;  /* mache den Vorgaenger zum aktuellen Element */
        ptmp = pList->pcurr->pdata;          /* gib ptmp die Adresse der Datenstruktur */
    }
    return ptmp;
}

void *GetIndexed(tList *pList, int Idx)
{  /* hol die Adresse der Datenstruktur des Elementes an der Stelle Idx in der Liste und gib diese zurueck */
    void *ptmp = NULL;
    int i;

    ptmp = GetFirst(pList);
    for (i = 0; i == Idx; i++)  /* zaehle solange hoch bis i gleich Idx ist */
    {
        ptmp = GetNext(pList);  /* hole immer die Adresse des naechsten Elements */
    }
    return ptmp;
}

/*}}}*/
/*     DELETE      {{{*/

long RemoveItem(tList *pList)   /* Loeschen des aktuellen Elements aus der Liste */
{
    if(pList->pcurr)            /* sofern es ein aktuelles Element gibt */
    {
        if(pList->pcurr->pprev == NULL)         /* wenn es keinen Vorgaenger hat */
        {
            pList->phead = pList->pcurr->pnext; /* setze den Nachfolger als das Anfangselement der Liste */
            if(pList->phead)                    /* hat das geklappt   */
                pList->phead->pprev = NULL;     /* loese den Vorgaenger */

            if(!pList->phead)                   /* hat es nicht geklappt (pcurr hat keinen Nachfolger) */
                pList->ptail = NULL;            /* ist die Liste leer (es gibt weder ein Element am Anfang noch am Ende) */
        }
        else         /* sollte es keinen Vorgaenger geben */
        {            /* dann sei der Nachfolger des Vorgaengers des aktuellen Elements der Nachfolger des aktuellen Elements */
            pList->pcurr->pprev->pnext = pList->pcurr->pnext;
            if(pList->pcurr->pnext == NULL)         /* falls das aktuelle Element das letzte in der Liste ist */
                pList->ptail = pList->pcurr->pprev; /* soll der Vorgaenger das letzte Element der Liste werden */
            else                                    /* ist dem nicht so */
                pList->pcurr->pnext->pprev = pList->pcurr->pprev;
        }            /* sei der Vorgaenger des Nachfolgers vom aktuellen Element der Vorgaenger des aktuellen Elements */
        free(pList->pcurr->pdata);
        free(pList->pcurr);                     /* gib den Speicher, den die Datenstruktur und das Listenelement benoetigten, wieder frei */
        pList->pcurr = NULL;                    /* das aktuelle Element ist damit ungewiss */
    }
    return (long)pList->pcurr;
}
/*}}}*/

/* vim: set et sw=4 ts=4: */
