
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

#define FILE_PATH "./data/storelist.txt"

void help_menue(void) {
    printf("\nSwitch\tParameter\tHilfe\n");
    printf("-h\t\t\tDisplay this help\n");
    printf("-f\t<filename>\tSwitch for setting filname.\n");
    printf("-p\t\t\tDisplay all ware (required -f)\n");
    printf("-s\t<BY_ARTNR | BY_ARTBEZ>\tSort by Artikelnummer\n\
            \t\t\tor Artikelname (required -p)\n\
            \t\t\tBY_ARTNR ist default option.\n");
    printf("-r\t\t\tsort reverse (required -s)\n");
    printf("-n\t<ARTNR> <ARTBEZ> <BSTAND>\n\
            \t\tNew dataset (required -f)\n");
    printf("-c\t<+-'int number'> <PATTER>\tZugang/Abgang(required -f)\n");
    printf("-d\t<PATTER>\tdelete dataset(required -f)\n");
}

int main(int argc, char *argv[]) {                          /* Programmstart */

    int i, j, argi = 1;
    char* filename = NULL;
    char* sort_pa = NULL;
    char* newDS[3];
    
    tList* storelist = NULL;
    char SET_FILE = 0x00, SET_PRINT = 0x00, SET_SORT = 0x00, SET_RSORT = 0x00;
    char SET_NEW = 0x00, SET_CHANGE = 0x00, SET_DELETE = 0x00;

    storelist = CreateList();                               /* erzeuge eine neue Liste       */

    if ( argc > 1 ) {
        while ( argc > argi ) {
            if ( ! ( strcmp( "-h", argv[argi] ) ) ) {
                help_menue();
                return EXIT_SUCCESS;

            } else if ( ( ! ( strcmp( "-f", argv[argi] ) ) ) 
                    && ( argv[argi+1] != NULL ) ) {
                argi++;
                filename = (char*)calloc( strlen(argv[argi])+1, sizeof(char) );
                strcpy( filename, argv[argi] );
                SET_FILE = 0x01;
            } else if ( ! ( strcmp( "-p", argv[argi] ) ) )  {
                SET_PRINT = 0x01;

            } else if ( ! ( strcmp( "-s", argv[argi] ) )
                    && ( argv[argi+1] != NULL ) ) {
                argi++;
                sort_pa = (char*)calloc( strlen( argv[argi] ) + 1, sizeof(char));
                strcpy(sort_pa, argv[argi]);
                SET_SORT = 0x01;

            } else if ( ! ( strcmp( "-r", argv[argi] ) ) ) {
                SET_RSORT = 0x01;

            } else if ( ( ! ( strcmp( "-n", argv[argi] ) ) )
                        && ( argv[argi+1] != NULL ) 
                        && ( argv[argi+2] != NULL ) 
                        && ( argv[argi+3] != NULL ) 
                      ) {
                argi++;
                for ( i=0; i <= 2; i++ ,argi++ ) {
                    newDS[i] = (char*)calloc( strlen( argv[argi] ) + 1, sizeof(char) );
                    strcpy( newDS[i], argv[argi] );
                }
                SET_NEW = 0x01;

            } else if ( ! strcmp( "-c", argv[argi] ) ) {
                SET_CHANGE = 0x01;
            } else if ( ! strcmp( "-d", argv[argi] ) ) {
                SET_DELETE = 0x01;
            } else {
                printf( "%s unknown\n", argv[argi] );
                help_menue();
                return EXIT_SUCCESS;
            }
            argi++;
        }
    } else {
        printf("no arguments\n");
    }


    if ( SET_FILE ) {
        printf( "File is set on %s\n", filename );
        if ( SET_PRINT ) {

            load( filename, storelist );
            print_all_list( storelist );

            if ( SET_SORT ) {
                if ( ! strcmp(sort_pa, "BY_ARTNR") ) {
                    if ( SET_RSORT ) {
                        sorteasyList( storelist , BY_ARTNR, DESC_ORDER);
                    } else {
                        sorteasyList( storelist , BY_ARTNR, ASC_ORDER);
                    }
                } else if  ( ! strcmp(sort_pa, "BY_ARTBEZ") ) {
                    if ( SET_RSORT ) {
                        sorteasyList( storelist , BY_ARTBEZ, DESC_ORDER );
                    } else {
                        sorteasyList( storelist , BY_ARTBEZ, ASC_ORDER );
                    }
                } else {
                    help_menue();
                    fprintf( stderr, "*** ERROR sortparameter %s unknown\n", sort_pa );
                    return EXIT_FAILURE;
                }
            }/* else {
                help_menue();
                printf("\n\nto few arguments\n");
            //    return EXIT_FAILURE;
            }*/

            print_all_list( storelist );

        } else if ( SET_NEW ) {
            load( filename, storelist );
            i = atoi(newDS[0]);
            j = atoi(newDS[2]);
            add_to_list( storelist, &i, newDS[1], &j );
        
            print_all_list( storelist );
            save_list( filename, storelist );
        }
    }  else {
        help_menue();
        fprintf( stderr, "\n\n*** ERROR not File set\n" );
        return EXIT_FAILURE;
    }

    if ( strcmp( filename, FILE_PATH ) ) {
        free(filename); 
    }

    return EXIT_SUCCESS;                                    /* das war's */
}
