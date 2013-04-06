
#ifndef __SPRITE_VIEWER_H__
#define __SPRITE_VIEWER_H__

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>

/******************************************************************************
		MACROS
******************************************************************************/
#ifndef NULL
	#define NULL (void*)0
#endif

#ifndef NEW_OBJECT
    #define NEW_OBJECT( type ) (type*)malloc( sizeof(type) )
#endif

#ifndef NEW_ARRAY
    #define NEW_ARRAY( type, amount ) (type*)calloc( amount, sizeof(type)*amount )
#endif

#ifndef FREE_MEMORY
    #define FREE_MEMORY( x ) free( x ); x = NULL
#endif

/******************************************************************************
		STRUCTURES
******************************************************************************/
typedef struct {
    bool is_sheet;
    bool use_alpha;
	int num_frames;
	int frame_delay;
	int width;
	int height;
	ALLEGRO_BITMAP** bitmap; /* Array of bitmaps */
	ALLEGRO_COLOR alpha;
} sprite_t;

/******************************************************************************
		INLINE FUNCTIONS
******************************************************************************/
static inline int get_max_i( int x, int y ) {
	return x > y ? x : y;
}

static inline int get_min_i( int x, int y ) {
	return x < y ? x : y;
}

static inline float get_max_f( float x, float y ) {
	return x > y ? x : y;
}

static inline float get_min_f( float x, float y ) {
	return x < y ? x : y;
}

#endif /* __SPRITE_VIEWER_H__ */
