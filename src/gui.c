/*****************************************************************************
 * gui.c                                                                     *
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
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

#include "ctrl.h"

static GtkWidget *list;             /* GTK Widget zum Darstellen der Listenelemente */
static GtkWidget *title_entry;      /* Eingabefelder für neue Elemente */
static GtkWidget *author_entry;
static GtkWidget *debitor_entry;
static GtkWidget *search_entry;     /* Eingabefeld für Suche   */
static GtkTreeSelection *selection; /* für Auswahl einer Zeile */

tList *booklist   = NULL;           /* Pointer auf Bücherliste */
tList *searchView = NULL;           /* Liste für Suchergebnisbetrachtung */

int searching = 0;                  /* Suchstatus */
int orders[4] = {ASC_ORDER,DESC_ORDER,DESC_ORDER,DESC_ORDER}; /* Reihenfolgen für Sortierung je Feld */
static int sortedBy = BY_ID;        /* Startsortierungsfeld */

enum store_elements                 /* Elemente des GTK ListStore */
{
    TITLE,
    AUTHOR,
    DEBITOR,
    DATE,
    ID,
    N_COL   /* Anzahl der ListStore Einträge */
};

/* entfernen eines Listeneintrags durch Auswahl */
static void remove_item(GtkWidget *widget, gpointer selection)
{
    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gint id;
    gchar* data;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));

    if (gtk_tree_model_get_iter_first(model, &iter) == 0)
        return;

    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection),  /* gibt es eine Auswahl */
                &model, &iter)){
        gtk_tree_model_get(model, &iter, ID, &id, -1);   /* hole die ID des Eintrags */
        remove_from_list(booklist, id);                  /* Entferne das Element mit dieser ID aus der Liste */
        gtk_list_store_remove(store, &iter);             /* Entferne Eintrag aus der Ansicht */
    }
}


static void load_list(tList *source)   /* Laedt Listen zur Darstellung */
{
    GtkListStore *store;
    GtkTreeIter iter;
    media *tmp;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    gtk_list_store_clear(store);           /* entferne alle Einträge aus der Ansicht */

    tmp = GetFirst(source);     /* Lauf durch die Liste */
    while (tmp){
        store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
        gtk_list_store_append(store, &iter);                  /* nimm jedes Element und hänge es in der Darstellung an */
        gtk_list_store_set(store, &iter, TITLE, tmp->title  ,
                                        AUTHOR, tmp->author ,
                                       DEBITOR, tmp->debitor,
                                          DATE, tmp->date,
                                            ID, tmp->id,
                                                  -1);
        tmp = GetNext(source);
    }
}

static void newSortedView(GtkWidget *widget, gpointer field)  /* erzeugt neusortierte Ansicht */
{
    tList *newView = NULL;
    int sort = *((int*)field);

    orders[sort] *= -1;   /* Reihenfolge vertauschen */
    sortedBy = sort;      /* Nach welchem Feld soll sortiert werden */

    newView = CreateList();   /* eine neue Liste zum hinein sortieren */
    sortList(booklist, newView, sort, orders[sort]);
    booklist = newView;      /* booklist sei jetzt unsere neusortiere Liste */
    load_list(booklist);     /* stelle die neusortierte Liste dar */
}

static void add_item(GtkWidget *widget, gpointer data) /* Füge eine Eintrag hinzu */
{

    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter iter;
    const char *title,*author, *debitor;
    char *date;
    int ident;
    tList* medialist = NULL;
    time_t timep;

    title   = author = debitor = NULL;
    title   = gtk_entry_get_text(GTK_ENTRY(title_entry));   /* hol den Text der Eingabefelder */
    author  = gtk_entry_get_text(GTK_ENTRY(author_entry));
    debitor = gtk_entry_get_text(GTK_ENTRY(debitor_entry));

    if (*title && *debitor)   /* falls es Eingaben für Titel und "An wen" gibt */
    {
        ident = get_new_id(data);    /* generiere eine neue ID */

        time(&timep);
        date = ctime(&timep);        /* erzeuge einen Zeitstempel */
        date[strlen(date)-1] = 0;

        medialist = CreateList();    /* um sortiert einfügen zu können */
        add_to_list(medialist, title, author, debitor, date, ident);
        sortList(medialist, booklist, sortedBy, orders[sortedBy]);
        DeleteList(medialist);

        load_list(booklist);         /* stelle dar */

        gtk_entry_set_text(GTK_ENTRY(title_entry),   "");  /* entferne den Text der Eingabefelder */
        gtk_entry_set_text(GTK_ENTRY(author_entry),  "");
        gtk_entry_set_text(GTK_ENTRY(debitor_entry), "");
    }
}

/* erstellt eine Liste abhängig vom gesuchten Wort */
static void search(GtkEntry *entry, GtkEntryIconPosition pos, GdkEvent *event, gpointer data)
{
    const char*  pattern = NULL;
    pattern = gtk_entry_get_text(GTK_ENTRY(search_entry));  /* nimm den Text aus dem Suchfeld */

    if (strcmp(pattern, "") && searching == 0)
    {   /* ist ein Text da und es werden gerade keine Suchergebnisse dargestellt */
        searching = 1;  /* jetzt werden Suchergebnisse dargestellt */
        gtk_entry_set_icon_from_icon_name(entry, pos, "editclear"); /* ändert das Symbol im Suchfeld */
        searchView = CreateList();   /* erstelle eine neue Liste */
        seachList(booklist, searchView, pattern); /* suche patter in booklist und kopiere diese in die neue Liste */
        load_list(searchView);    /* stell die Liste mit den Suchergebnissen dar */
    }
    else /* steht kein Text im Suchfeld oder soll eine neue Suche gestartet werden */
    {
        if (searching == 1)
            DeleteList(searchView);  /* lösche die alten Suchergebnisse */
        searching = 0;               /* jetzt wird die ganze Liste dargestelllt */
        gtk_entry_set_text(GTK_ENTRY(search_entry), "");  /* entferne den Text aus dem Suchfeld */
        gtk_entry_set_icon_from_icon_name(entry, pos, "find"); /* setzte das Symbold im Suchfeld auf "Lupe" */
        load_list(booklist);         /* stell wieder die ganze Liste dar */
    }
}


static GtkWidget *init_list(tList *source) /* baut ein Listenansichtfenster */
{
    GtkWidget         *sc_win;
    GtkListStore      *store;
    GtkCellRenderer   *cell;
    GtkTreeViewColumn *column;

    sc_win = gtk_scrolled_window_new(NULL, NULL);  /* ein neues Fenster */
    store  = gtk_list_store_new(N_COL, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT); /* welche Spalten wird es geben */
    list   = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));  /* aus dem store erzugtes Listenmodel (Zeilen) */
    cell   = gtk_cell_renderer_text_new();       /* in den Zellen der können wir Text darstellen */

    static int by_id      = BY_ID;       /* und das Sortierfeld zu übergeben */
    static int by_title   = BY_TITLE;
    static int by_author  = BY_AUTHOR;
    static int by_debitor = BY_DEBITOR;

    column = gtk_tree_view_column_new_with_attributes("Nr.",   /* Spaltenkopf für ID */
                                                      cell,
                                                      "text",
                                                      ID,
                                                      NULL
                                                     );
    gtk_tree_view_column_set_clickable(column, 1);            /* dieser sei klickbar */
    g_signal_connect(column, "clicked", G_CALLBACK(newSortedView), &by_id); /* falls er angeklickt wird sortiere nach ID */
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);  /* häng die Einträge aus der Liste für die Spalte an */

    column = gtk_tree_view_column_new_with_attributes("Title",  /* Spaltenkpf für Titel */
                                                      cell,
                                                      "text",
                                                      TITLE,
                                                      NULL
                                                     );
    gtk_tree_view_column_set_clickable(column, 1);           /* dieser sei Klickbar */
    g_signal_connect(column, "clicked", G_CALLBACK(newSortedView), &by_title); /* falls angeklickt wird sortiere nach Titel */
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);  /* häng die Einträge aus der Liste für die Spalte an */

    column = gtk_tree_view_column_new_with_attributes("Author/Interpret", /* Spaltenkopf für Author */
                                                      cell,
                                                      "text",
                                                      AUTHOR,
                                                      NULL
                                                     );
    gtk_tree_view_column_set_clickable(column, 1);           /*dieser sei klickbar */
    g_signal_connect(column, "clicked", G_CALLBACK(newSortedView), &by_author); /* falls er angeklickt wird sortiere nach Author */
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);  /* häng die Einträge aus der Liste für die Spalte an */

    column = gtk_tree_view_column_new_with_attributes("to whom", /* Spaltenkopf für Debitor */
                                                      cell,
                                                      "text",
                                                      DEBITOR,
                                                      NULL
                                                     );
    gtk_tree_view_column_set_clickable(column, 1);              /* dieser sei klickbar */
    g_signal_connect(column, "clicked", G_CALLBACK(newSortedView), &by_debitor); /* falls er angeklickt wird sortiere nach Debitor */
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);  /* häng die Einträge aus der Liste für die Spalte an */

    column = gtk_tree_view_column_new_with_attributes("Date",
                                                      cell,
                                                      "text",
                                                      DATE,
                                                      NULL
                                                     );
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);  /* häng die Einträge aus der Liste für die Spalte an */

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_win),
                                   GTK_POLICY_NEVER,  /* kein seitwärtiges scrolling */
                                   GTK_POLICY_AUTOMATIC); /* automatisches vertikales scrolling */

    load_list(booklist);  /* pack die Liste in die Darstellung */
    gtk_container_add(GTK_CONTAINER(sc_win), list);  /* füge list dem Fenster hinzu */

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list)); /* ermögliche Auswahl von Zeilen */

    g_object_unref(G_OBJECT(store));

    return sc_win;
}

int draw_gui(tList *source)  /* erzeugt garphische Nutzerschnittstelle mit allen Buttons, Eingabefeldern, etc. */
{
    GtkWidget *window, *vbox, *listView, *entry_container_box, *button_box;
    GtkWidget *entry_box, *add_button, *add_img, *remove_button, *trash_img;
    GtkWidget *title_label, *author_label, *debitor_label;
    GtkWidget *title_box, *author_box, *debitor_box;

    booklist = source;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);   /* ein neues Fenster */
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); /* in der mitte vom Bildschirm */
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);       /* der Größe 600x600 px */
    gtk_window_set_title(GTK_WINDOW(window), "BookList");            /* mit dem Titel "Booklist" */
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);  /* wird dieses Fenster geschlossen soll sich das Programm beenden */

    vbox = gtk_vbox_new(0, 2);  /* eine vbox wird dem Fenster hinzugefügt */
    gtk_container_add(GTK_CONTAINER(window), vbox);

    search_entry = gtk_entry_new();  /* das Suchfeld wird eingefügt */
    gtk_entry_set_text(GTK_ENTRY(search_entry), "");
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(search_entry),
                                      GTK_ENTRY_ICON_SECONDARY,
                                      "find"
                                     );
    gtk_box_pack_start(GTK_BOX(vbox), search_entry, 0, 0, 2);
    g_signal_connect(GTK_ENTRY(search_entry), "icon-press", G_CALLBACK(search), NULL); /* wird dass Icon im Suchfeld angeklickt starte "search" */

    listView = init_list(booklist);  /* die Listen ansicht wird eingefügt */
    gtk_box_pack_start(GTK_BOX(vbox), listView, 1, 1, 2);

    entry_container_box = gtk_hbox_new(0, 1);  /* für die weiteren Eingabefelder und Buttons */
    gtk_box_pack_start(GTK_BOX(vbox), entry_container_box, 0, 0, 1);

    entry_box = gtk_vbox_new(0, 1); /* box für Eingabefelder */
    gtk_box_pack_start(GTK_BOX(entry_container_box), entry_box, 1, 1, 2);

    title_label   = gtk_label_new("Title");    /* Bezeichnunger vor den Eingabefeldern */
    author_label  = gtk_label_new("Author");
    debitor_label = gtk_label_new("To Whom");

    title_entry   = gtk_entry_new();           /* Eingabefelder für Titel */
    gtk_entry_set_text(GTK_ENTRY(title_entry), "");
    gtk_entry_set_max_length(GTK_ENTRY(title_entry),   128);
    author_entry  = gtk_entry_new();                             /* Author */
    gtk_entry_set_text(GTK_ENTRY(author_entry), "");
    gtk_entry_set_max_length(GTK_ENTRY(author_entry),  128);
    debitor_entry = gtk_entry_new();                         /* und Debitor */
    gtk_entry_set_text(GTK_ENTRY(debitor_entry), "");
    gtk_entry_set_max_length(GTK_ENTRY(debitor_entry), 128);

    title_box = gtk_hbox_new(0, 1);  /* alles schön verpacken */
    gtk_box_pack_start(GTK_BOX(title_box), title_label,     0, 1, 2);
    gtk_box_pack_start(GTK_BOX(title_box), title_entry,     1, 1, 2);
    gtk_box_pack_start(GTK_BOX(entry_box), title_box,       0, 0, 2);

    author_box = gtk_hbox_new(0, 1);
    gtk_box_pack_start(GTK_BOX(author_box), author_label,   0, 1, 2);
    gtk_box_pack_start(GTK_BOX(author_box), author_entry,   1, 1, 2);
    gtk_box_pack_start(GTK_BOX(entry_box),  author_box,     0, 0, 2);

    debitor_box = gtk_hbox_new(0, 1);
    gtk_box_pack_start(GTK_BOX(debitor_box), debitor_label, 0, 1, 2);
    gtk_box_pack_start(GTK_BOX(debitor_box), debitor_entry, 1, 1, 2);
    gtk_box_pack_start(GTK_BOX(entry_box),   debitor_box,   0, 0, 2);

    button_box = gtk_vbox_new(1, 1); /* auch die Buttons in den entry_container */
    gtk_box_pack_start(GTK_BOX(entry_container_box), button_box,  0, 0, 2);

    add_button = gtk_button_new();  /* Button mit Plus-Symbol */
    add_img = gtk_image_new_from_file("./data/plus.png");
    gtk_button_set_image(GTK_BUTTON(add_button), add_img);       /* wird er angeklickt soll add_item aufgerufen werden */
    gtk_box_pack_start(GTK_BOX(button_box), add_button, 1, 1, 1);
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_item), booklist);

    remove_button = gtk_button_new(); /* Button mit Eimer-Symbol */
    trash_img = gtk_image_new_from_file("./data/trash.png");
    gtk_button_set_image(GTK_BUTTON(remove_button), trash_img);      /* wird er angeklickt soll */
    gtk_box_pack_start(GTK_BOX(button_box), remove_button, 1, 1, 1); /*der ausgewählte Eintrag mit remove_item entfernt werden */
    g_signal_connect(remove_button, "clicked", G_CALLBACK(remove_item), selection);


    gtk_widget_show_all(window);  /* Auf die Leinwand */

    return 0;
}

/* vim: set et sw=4 ts=4: */
