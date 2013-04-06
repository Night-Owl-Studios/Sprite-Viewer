/* 
 * File:   loader.h
 * Author: hammy
 *
 * Created on April 3, 2013, 7:30 PM
 */

#ifndef __SPRITE_LOADER_H__
#define	__SPRITE_LOADER_H__

#include "sprite_viewer.h"

sprite_t* load_sprite( ALLEGRO_PATH* path, ALLEGRO_CONFIG* cfg );

void destroy_sprite( sprite_t* sprite );

#endif	/* __SPRITE_LOADER_H__ */
