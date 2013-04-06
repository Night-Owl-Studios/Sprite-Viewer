/* 
 * File:   util_functions.h
 * Author: hammy
 *
 * Created on April 3, 2013, 10:06 PM
 */

#ifndef __UTIL_FUNCTIONS_H__
#define	__UTIL_FUNCTIONS_H__

#include <stdbool.h>

void print_log( const char* str, ... );
void print_err( const char* str, ... );
void print_ok( const char* str, ... );
void print_msg( const char* str, ... );

bool file_exists( const char* filename );

#endif	/* __UTIL_FUNCTIONS_H__ */

