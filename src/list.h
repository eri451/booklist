/* list.h */

#define OK 1
#define FAIL 0
#define DESC_ORDER -1
#define ASC_ORDER 1
/*-------------------------------------------------------------------------*/
/* Prototypen fuer die Funktionen                                          */

typedef struct tCONNECT {
    struct tCONNECT *pnext;
    struct tCONNECT *pprev;
    void            *pdata;
} tcnct;

typedef struct {
    tcnct *phead;
    tcnct *ptail;
    tcnct *pcurr;
} tList;

tList * CreateList(void);                           /* erzeuge leere Liste */
long    DeleteList(tList* pList);                   /* loesche leere Liste */

long  InsertHead     (tList* pList, void *pItemIns);/* fuege vorn ein      */
long  InsertTail     (tList* pList, void *pItemIns);/* fuege hinten ein    */
long  InsertSorted   (tList* pList,                 /* fuege sortiert ein  */
                      void  *pItemIns,
                      int    field,
                      int    order,
                      int  (*compare)(int, void*, void*)
                     );
long  InsertBehind   (tList* pList, void *pItemIns);/* fuege ein hinter %  */
long  InsertBefore   (tList* pList, void *pItemIns);/* fuege ein vor    %  */

void* GetSelected    (tList* pList);                /* gib aktuellen DS    */
void* GetFirst       (tList* pList);                /* gib ersten DS       */
void* GetLast        (tList* pList);                /* gib letzten DS      */
void* GetNext        (tList* pList);                /* gib naechsten DS    */
void* GetPrev        (tList* pList);                /* gib vorigen DS      */
void* GetIndexed     (tList* pList,int Idx);        /* gib DS lt. Index    */

long  RemoveItem     (tList* pList);                /* loesche %           */

/* % steht fuer aktuellen Satz                                             */
