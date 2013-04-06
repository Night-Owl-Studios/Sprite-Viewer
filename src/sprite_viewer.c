
/* Loading sprites using config files and sprite sheets */

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>

#include "sprite_viewer.h"
#include "sprite_loader.h"
#include "util_functions.h"
#include "sheet_exporter.h"

/******************************************************************************
        GLOBAL VARIABLES
 ******************************************************************************/
static const int DISPLAY_FPS = 60;
static const int DISPLAY_WIDTH = 640;
static const int DISPLAY_HEIGHT = 480;
static const char* config_file = "viewer_settings.bin";

/******************************************************************************
        FUNCTION PROTOTYPES
 ******************************************************************************/
void init( ALLEGRO_DISPLAY**, int* fps );
void do_main_loop( ALLEGRO_DISPLAY**, int fps, const sprite_t* sprite );
void draw_sprite( ALLEGRO_DISPLAY*, const sprite_t*, int frame_num);
void prevent_tiny_display( ALLEGRO_DISPLAY*, const sprite_t* );

/******************************************************************************
        PROGRAM INITIALIZATION
 ******************************************************************************/
void init( ALLEGRO_DISPLAY** display, int* fps ) {
    ALLEGRO_CONFIG* cfg = NULL;
    int width = DISPLAY_WIDTH;
    int height = DISPLAY_HEIGHT;
    *fps = DISPLAY_FPS;

    al_set_org_name("Night Owl Studios");
    al_set_app_name("Sprite Viewer");

    assert(al_init());
    assert(al_init_image_addon());
    assert(al_install_keyboard());

    /* Load the program settings from config_file */
    cfg = al_load_config_file(config_file);
    if (cfg) {
        /* Read in values from the config if it had loaded correctly */
        width   = atoi(al_get_config_value(cfg, NULL, "display_width"));
        height  = atoi(al_get_config_value(cfg, NULL, "display_height"));
        *fps    = atoi(al_get_config_value(cfg, NULL, "display_fps"));
        /* Verify that all the values were read in correctly from the config */
        width   = get_max_i(width, DISPLAY_WIDTH);
        height  = get_max_i(height, DISPLAY_HEIGHT);
        *fps    = get_max_i(*fps, DISPLAY_FPS);
        al_destroy_config(cfg);
    }

    al_set_new_display_flags(ALLEGRO_RESIZABLE);
    *display = al_create_display(width, height);
    assert(*display != NULL);
}

/******************************************************************************
        GAME LOOP
 ******************************************************************************/
void do_main_loop( ALLEGRO_DISPLAY** win, int fps, const sprite_t* sprite ) {
    bool running = true;
    bool redraw = true;
    int curr_frame = 0;
    int frame_iter = 0;
    ALLEGRO_EVENT_QUEUE* event_queue = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_DISPLAY* display = *win;
    ALLEGRO_EVENT event;

    event_queue = al_create_event_queue();
    timer = al_create_timer(1.f / fps);
    assert(event_queue);
    assert(timer);

    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));

    al_start_timer(timer);

    while (running) {
        al_wait_for_event(event_queue, &event);

        switch (event.type) {
                /* Determine if a redraw should be performed */
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                if (frame_iter++ >= sprite->frame_delay) {
                    if (++curr_frame >= sprite->num_frames) {
                        curr_frame = 0;
                    }
                    frame_iter = 0;
                }
                break;
                /* Send keyboard information to the input system */
            case ALLEGRO_EVENT_KEY_UP:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    running = false;
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    export_to_sheet( sprite );
                }
                break;
                /* Handle all display events */
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                if (!al_acknowledge_resize(display))
                    continue;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                running = false;
                break;
        }

        if (redraw == true && al_event_queue_is_empty(event_queue)) {
            prevent_tiny_display( display, sprite );
            
            /* Drawing Ops go here */
            draw_sprite( display, sprite, curr_frame );
            al_flip_display();
            al_clear_to_color(al_map_rgb(255, 255, 255));
        }
    }

    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
}

/******************************************************************************
        PREVENTING TINY DISPLAY
 ******************************************************************************/
void prevent_tiny_display( ALLEGRO_DISPLAY* display, const sprite_t* sprite ) {
    int width = al_get_display_width( display );
    int height = al_get_display_height( display );
    
    if ( width < sprite->width || height < sprite->height )
        al_resize_display( display, width, height );
}

/******************************************************************************
        SPRITE_DRAWING
 ******************************************************************************/
void draw_sprite( ALLEGRO_DISPLAY* display, const sprite_t* sprite, int frame_num ) {
    float width = get_max_i(al_get_display_width(display), sprite->width);
    float height = get_max_i(al_get_display_height(display), sprite->height);

    if (width > height) width = height;
    if (height > width) height = width;
    
    /* Draw either a sprite image or a sprite sheet */
    if ( sprite->is_sheet ) {
        al_draw_scaled_bitmap(
            sprite->bitmap[ 0 ],
            sprite->width*frame_num, 0,
            sprite->width, sprite->height,
            0.f, 0.f, width, height * (sprite->height / sprite->width),
            0
        );
    }
    else {
        al_draw_scaled_bitmap(
            sprite->bitmap[ frame_num ],
            0.f, 0.f, sprite->width, sprite->height,
            0.f, 0.f, width, height * (sprite->height / sprite->width),
            0
        );
    }
}

/******************************************************************************
        MAIN
 ******************************************************************************/
int main() {
    const char* file            = NULL;
    int target_fps              = DISPLAY_FPS;
    ALLEGRO_CONFIG* cfg         = NULL;
    ALLEGRO_FILECHOOSER* dlg    = NULL;
    ALLEGRO_PATH* path          = NULL;
    sprite_t* sprite            = NULL;
    ALLEGRO_DISPLAY* display    = NULL;
    ALLEGRO_BITMAP* icon        = NULL;
    
    /* Initialize the display and set the icon */
    init( &display, &target_fps);
    icon = al_load_bitmap( "icon.png" );
    if ( icon )
        al_set_display_icon( display, icon );

    /* Create and display a file-input dialog box */
    dlg = al_create_native_file_dialog(
        NULL, "Choose Sprite Config File", "*.ini",
        ALLEGRO_FILECHOOSER_FILE_MUST_EXIST
    );
    assert(dlg);
    
    al_show_native_file_dialog(display, dlg);
    if ( !al_get_native_file_dialog_count( dlg ) ) {
        al_destroy_native_file_dialog(dlg);
        al_destroy_bitmap( icon );
        al_destroy_display( display );
        return 0;
    }

    /* Load the file path from the input file */
    path = al_create_path( al_get_native_file_dialog_path(dlg, 0) );
    al_destroy_native_file_dialog(dlg);
    
    /* Quit if no file was selected from the input dialog */
    file = al_path_cstr( path, ALLEGRO_NATIVE_PATH_SEP );
    assert( file );

    cfg = al_load_config_file( file );
    if (!cfg) {
        print_err(
            "Unable to load the sprite's configuration data from %s. "\
			"Please check that the file exists and is not corrupted.\n",
            file
        );
        al_destroy_path( path );
        al_destroy_bitmap( icon );
        al_destroy_display( display );
        return 0;
    }
    
    sprite = load_sprite( path, cfg );
    if ( sprite )
        do_main_loop( &display, target_fps, sprite );

    al_destroy_path( path );
    if ( sprite ) destroy_sprite( sprite );
    al_destroy_config( cfg );
    if ( icon ) al_destroy_bitmap( icon );
    al_destroy_display( display );
    return 0;
}
