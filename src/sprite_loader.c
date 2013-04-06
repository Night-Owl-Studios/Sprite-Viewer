
#include "util_functions.h"
#include "sprite_loader.h"

/******************************************************************************
        FORWARD DECLARATIONS
******************************************************************************/
bool load_sprite_sheet( ALLEGRO_PATH*, ALLEGRO_CONFIG*, sprite_t* );
bool load_sprite_images( ALLEGRO_PATH*, ALLEGRO_CONFIG*, sprite_t* );

/******************************************************************************
		LOADING SPRITE CONFIGURATION
******************************************************************************/
sprite_t* load_sprite( ALLEGRO_PATH* path, ALLEGRO_CONFIG* cfg ) {
	int use_alpha       = 0;
	int is_sheet        = 0;
	int frame_delay     = 0;
	unsigned alpha_r    = 0;
	unsigned alpha_g    = 0;
	unsigned alpha_b    = 0;
	int sprite_width    = 100;
	int sprite_height   = 100;
    sprite_t* sprite    = NULL;
	
	is_sheet        = atoi( al_get_config_value( cfg, NULL, "is_sheet" ) );
	frame_delay     = atoi( al_get_config_value( cfg, NULL, "frame_delay" ) );
	use_alpha       = atoi( al_get_config_value( cfg, NULL, "use_alpha" ) );
	
	if ( use_alpha > 0 ) {
		alpha_r     = atoi( al_get_config_value( cfg, "ALPHA", "r" ) );
		alpha_g     = atoi( al_get_config_value( cfg, "ALPHA", "g" ) );
		alpha_b     = atoi( al_get_config_value( cfg, "ALPHA", "b" ) );
	}
	
	sprite_width    = atoi(al_get_config_value( cfg, "SIZE", "width" ));
	sprite_height   = atoi(al_get_config_value( cfg, "SIZE", "height" ));
	
	if ( sprite_width < 1 || sprite_height < 1 ) {
		print_err(
			"The sprite width and/or height values are invalid. "\
			"Please ensure that the sprite sizes are greater than zero.\n"
		);
		return false;
	}
	
    sprite = NEW_OBJECT( sprite_t );
	sprite->width = sprite_width;
	sprite->height = sprite_height;
	sprite->frame_delay = frame_delay;
	sprite->alpha = al_map_rgb( alpha_r, alpha_g, alpha_b );
    sprite->is_sheet = is_sheet;
    sprite->use_alpha = (use_alpha > 0);
	
	if ( is_sheet ) {
		if ( !load_sprite_sheet( path, cfg, sprite ) ) {
            FREE_MEMORY( sprite );
        }
    }
	else {
        if ( !load_sprite_images( path, cfg, sprite ) ) {
            FREE_MEMORY( sprite );
        }
    }
    
    return sprite;
}

/******************************************************************************
		LOADING SPRITE DATA (single sprite sheet)
******************************************************************************/
bool load_sprite_sheet(
    ALLEGRO_PATH* path,
    ALLEGRO_CONFIG* cfg,
    sprite_t* sprite
) {
    ALLEGRO_CONFIG_ENTRY* cfg_iter = NULL;
    const char* filename =
        al_get_config_value(
            cfg, "FILES", al_get_first_config_entry( cfg, "FILES", &cfg_iter )
        );
    
    al_set_path_filename( path, filename );
    filename            = al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP );
    sprite->num_frames  = atoi( al_get_config_value(cfg, NULL, "num_frames" ));
    sprite->bitmap      = NEW_OBJECT( ALLEGRO_BITMAP* );
    sprite->bitmap[0]   = al_load_bitmap( filename );
    
    if ( !sprite->bitmap[0] ) {
        print_err(
            "Unable to allocate memory for %s. "\
            "Please ensure the input image is a reasonable size.",
            filename
        );
        FREE_MEMORY( sprite->bitmap );
        return false;
    }
    
    if ( sprite->use_alpha )
        al_convert_mask_to_alpha(sprite->bitmap[0], sprite->alpha);
    
    return true;
}

/******************************************************************************
		LOADING SPRITE DATA (individual images)
******************************************************************************/
bool load_sprite_images(
    ALLEGRO_PATH* path,
    ALLEGRO_CONFIG* cfg,
    sprite_t* sprite
) {
	int num_files = 0;
	int frame_iter = 0;
	ALLEGRO_CONFIG_ENTRY* cfg_iter = NULL;
	/* Just load the first section. Don't bother with config entries */
	const char* sheet_file = al_get_first_config_entry(
		cfg, "FILES", &cfg_iter
	);
	
	if ( !sheet_file ) {
		print_err(
			"No file name for a sprite sheet was listed under the "\
			"[FILES] section of the config file.\n"
		);
		return false;
	}
	
	/* tabulate the number of sprite frames */
    do {
		++num_files;
    }
	while ( al_get_next_config_entry( &cfg_iter ) );
    
	/* return the cfg iterator to the first entry under [FILES] */
	sheet_file = al_get_first_config_entry( cfg, "FILES", &cfg_iter );
	
	/* allocate enough space for each of the sprite frames */
	sprite->bitmap = NEW_ARRAY(ALLEGRO_BITMAP*, num_files);
	sprite->num_frames = num_files;
    
	/* iterate through each index in sprite->bitmap[] and load the images */
	while ( sheet_file ) {
        const char* filename = al_get_config_value( cfg, "FILES", sheet_file );
        al_set_path_filename( path, filename );
		sprite->bitmap[ frame_iter ] = al_load_bitmap(
            al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP )
        );
		
		if ( !sprite->bitmap[ frame_iter ] ) {
			print_err(
				"Unable to load a sprite file \"%s\" referenced from "\
				"the input config file. Aborting.\n",
                al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP )
			);
			destroy_sprite( sprite );
			return false;
		}
        /* Determine if the image should use an embedded alpha channel */
        if ( sprite->use_alpha )
            al_convert_mask_to_alpha(sprite->bitmap[ frame_iter ], sprite->alpha);
        
		sheet_file = al_get_next_config_entry( &cfg_iter );
		++frame_iter;
	}
	
	return true;
}

/******************************************************************************
		UNLOADING SPRITE DATA
******************************************************************************/
void destroy_sprite( sprite_t* sprite ) {
	if ( !sprite )
		return;
	
    if ( sprite->bitmap ) {
        if ( sprite->is_sheet ) {
            al_destroy_bitmap( sprite->bitmap[0] );
        }
        else {
            for ( int i = 0; i < sprite->num_frames; ++i ) {
                al_destroy_bitmap( sprite->bitmap[ i ] );
            }
        }
    }
	
	free( sprite->bitmap );
    free( sprite );
    sprite = NULL;
}
