
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include "sprite_viewer.h"
#include "util_functions.h"
#include "sheet_exporter.h"

static const char* BITMAP_EXPORT_FORMAT = ".png";

/******************************************************************************
 *      FORWARD DECLARATIONS
 ******************************************************************************/
bool save_sprite_sheet( ALLEGRO_PATH*, const sprite_t* );
bool save_sheet_config( ALLEGRO_PATH*, const sprite_t* );

/******************************************************************************
 *      SPRITE SHEET EXPORT SETUP
 ******************************************************************************/
bool export_to_sheet( const sprite_t* sprite  ) {
    bool ret = false;
    const char* filename = NULL;
    ALLEGRO_PATH* path = NULL;
    ALLEGRO_FILECHOOSER* dlg = NULL;
    
    /* Don't save sprite sheets to another sprite sheet... yet? */
    if ( sprite->is_sheet ) {
        al_show_native_message_box(
            NULL, "Error", "Saving of sprite sheets has been disabled.",
            "The sprite viewer can only export individual frames to a sheet.",
            "Cancel", ALLEGRO_MESSAGEBOX_ERROR
        );
        return false;
    }
    
    /* Setup an initial path to save the sprite sheet */
    path = al_get_standard_path( ALLEGRO_EXENAME_PATH );
    if ( path == NULL )
        return false;
    
    /* Setup a dialog box so the sprite sheet can be saved visually */
    dlg = al_create_native_file_dialog(
        al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP ),
        "Enter a file name to save the sprite sheet",
        "*.*",
        ALLEGRO_FILECHOOSER_SAVE
    );
    
    if ( dlg == NULL )
        return false;
    al_destroy_path( path );
    path = NULL;
    
    /* Display the dialog box and retrieve the requested file name */
    al_show_native_file_dialog( NULL, dlg );
    filename = al_get_native_file_dialog_path( dlg, 0 );
    
    /* Quit if no file was requested or the "cancel" button was pressed */
    if ( !filename ) {
        al_destroy_native_file_dialog( dlg );
        return true;
    }
    
    /* Setup a path structure for the file to be saved to */
    path = al_create_path( filename );
    al_destroy_native_file_dialog( dlg );
    if ( !path ) {
        print_err( "Unable to save a sprite sheet due to an internal path error");
        return false;
    }
    
    /* Save the sprite sheet and a corresponding config file */
    ret =   !save_sprite_sheet( path, sprite )
        ||  !save_sheet_config( path, sprite );
    
    if ( !ret ) {
        print_ok(
            "Successfully saved the sprite sheet to %s.",
            al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP )
        );
    }
    
    al_destroy_path( path );
    
    return ret;
}

/******************************************************************************
 *      SPRITE SHEET EXPORTING -- SAVE THE SHEET
 ******************************************************************************/
bool save_sprite_sheet( ALLEGRO_PATH* path, const sprite_t* sprite ) {
    bool ret = false;
    const char* filename = NULL;
    ALLEGRO_BITMAP* output = NULL;
    
    al_set_path_extension( path, BITMAP_EXPORT_FORMAT );
    filename = al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP );
    
    /* Check if the file already exists */
    if ( file_exists( filename ) ) {
        if ( al_show_native_message_box(
            NULL, "Overwrite file?", filename,
            "An image with this name already exists. "\
            "Would you like to overwrite it?", NULL, ALLEGRO_MESSAGEBOX_YES_NO
        ) == 1 )
        return false;
    }
    
    /* Prepare the sprite sheet! */
    output = al_create_bitmap(
        sprite->width * sprite->num_frames, sprite->height
    );
    if ( !output ) {
        print_err(
            "Unable to export the sprite sheet. Perhaps the images are too big?"
        );
        return false;
    }
    
    /* Prep the new bitmap for drawing */
    al_set_target_bitmap( output );
    if ( sprite->use_alpha == false ) /* Use the alpha color defined by the user */
        al_clear_to_color( al_map_rgba( 255, 255, 255, 255 ) ); /* white */
    else /* use the alpha channel build into the image */
        al_clear_to_color( sprite->alpha );
    
    /* Print the individual sprite frames to the sheet */
    for ( int i = 0; i < sprite->num_frames; ++i ) {
        al_draw_bitmap( sprite->bitmap[ i ], i*sprite->width, 0, 0 );
    }
    
    /* Save the new sprite sheet to a file */
    ret = al_save_bitmap ( filename, output );
    
    if ( !ret ) {
        print_err(
            "An I/O error occurred while saving the sprite sheet image to %s."\
            " Please check the file name and ensure that the disk is not "\
            "write protected of out of space.",
            filename
        );
    }
    
    /* Reset all draw commands to the current display */
    al_set_target_backbuffer( al_get_current_display() );
    al_destroy_bitmap( output );
    
    return ret;
}

/******************************************************************************
 *      SPRITE SHEET EXPORTING -- SAVE THE CONFIG
 ******************************************************************************/
bool save_sheet_config( ALLEGRO_PATH* path, const sprite_t* sprite ) {
    FILE* file = NULL;
    
    /* Set the path extension to *.ini in order to save a config */
    al_set_path_extension( path, ".ini" );
    
    file = fopen( al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP ), "w" );
    if ( !file ) {
        print_err( "Unable to save a config file for the requested sprite" );
        fclose( file );
        return false;
    }
    
    /* Reset the path extension back to png in order to save the filename */
    al_set_path_extension( path, BITMAP_EXPORT_FORMAT );
    
    /* Config Header */
    fprintf( file,
        "use_alpha=%i\n"\
        "is_sheet=1\n"\
        "frame_delay=%i\n"\
        "num_frames=%i\n\n",
        sprite->use_alpha, sprite->frame_delay, sprite->num_frames
    );
    
    /* Alpha Section */
    fprintf( file,
        "[ALPHA]\n"\
        "r=%i\n"\
        "g=%i\n"\
        "b=%i\n\n",
        (int)floor( (255.f*sprite->alpha.r) + 0.5f ),
        (int)floor( (255.f*sprite->alpha.g) + 0.5f ),
        (int)floor( (255.f*sprite->alpha.b) + 0.5f )
    );
    
    /* Size Section */
    fprintf( file,
        "[SIZE]\n"\
        "width=%i\n"\
        "height=%i\n\n",
        sprite->width, sprite->height
    );
    
    /* Files Section */
    
    fprintf( file,
        "[FILES]\n"\
        "file0=%s\n",
        al_get_path_filename( path )
    );
    
    return fclose( file ) == 0;
}
