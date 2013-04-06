
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "util_functions.h"

/******************************************************************************
 * ERROR PRINTING
******************************************************************************/
void print_log( const char* str, ... ) {
    va_list args;
    fprintf( stdout, "%9d --- ", (float)clock()/CLOCKS_PER_SEC );
    
    va_start( args, str );
        vfprintf( stdout, str, args );
    va_end( args );
}

/******************************************************************************
 * ERROR PRINTING
******************************************************************************/
void print_err( const char* str, ... ) {
    int num_chars = 0;
    int print_amt = 0;
    char* buffer = NULL;
    va_list args;
    
    fprintf( stdout, "Error\n" );
    
    va_start( args, str );
        /* Get the number of characters that should be printed */
        print_amt = vsnprintf( NULL, 0, str, args );
        /* Leave the function if there was an error while printing */
        if (print_amt < 0)
            return;
        /* Increment the number of characters that should be printed */
        num_chars += print_amt;
    va_end( args );
    
    /* Create a buffer to store the data within "str" */
    buffer = (char*)calloc( num_chars, num_chars*sizeof( char ) );
    va_start( args, str );
        vsnprintf( buffer, num_chars, str, args );
    va_end( args );
    
    /* create a message box that displays the data within "buffer" */
    al_show_native_message_box(
        NULL, "Error", "Runtime Error", buffer, NULL, ALLEGRO_MESSAGEBOX_ERROR
    );
    
    free( buffer );
}

/******************************************************************************
 * SUCCESS PRINTING
******************************************************************************/
void print_ok( const char* str, ... ) {
    int num_chars = 0;
    int print_amt = 0;
    char* buffer = NULL;
    va_list args;
    
    fprintf( stdout, "Success\n" );
    
    va_start( args, str );
        /* Get the number of characters that should be printed */
        print_amt = vsnprintf( NULL, 0, str, args );
        /* Leave the function if there was an error while printing */
        if (print_amt < 0)
            return;
        /* Increment the number of characters that should be printed */
        num_chars += print_amt;
    va_end( args );
    
    /* Create a buffer to store the data within "str" */
    buffer = (char*)calloc( num_chars, num_chars*sizeof( char ) );
    va_start( args, str );
        vsnprintf( buffer, num_chars, str, args );
    va_end( args );
    
    /* create a message box that displays the data within "buffer" */
    al_show_native_message_box(
        NULL, "Success", "Program Success", buffer, NULL, ALLEGRO_MESSAGEBOX_WARN
    );
    
    free( buffer );
}

/******************************************************************************
 * PRINTING TO STANDARD OUTPUT
******************************************************************************/
void print_msg( const char* str, ... ) {
    va_list args;
    va_start( args, str );
    
    vfprintf( stdout, str, args );
    
    va_end( args );
}

/******************************************************************************
 * DETERMINING IF FILES ALREADY EXIST
******************************************************************************/
bool file_exists( const char* filename ) {
    FILE* file = fopen( filename, "r" );
    
    if ( !file )
        return false;
    
    fclose( file );
    return true;
}
