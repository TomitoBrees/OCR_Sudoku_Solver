#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <string.h>

#include "edge_detection.h"
#include "pre_processing.h"
#include "rotate.h"

/* STRUCTURES */

typedef enum State
{
    START,
    PRE_PROCESSING,
    ROTATION,
    DETECTION,
    AI,
    CONSTRUCTION
} State;

typedef struct Widgets
{
    GtkWidget *main_window;
    GtkWidget *pre_process_button;
    GtkWidget *image_rotation_button;
    GtkWidget *construction_button;
    GtkWidget *detection_button;
    GtkWidget *ai_button;
    GtkWidget *save_button;
    GtkWidget *main_title;
    GtkWidget *image;
    GtkWidget *image_box;
    GtkWidget *manual_text;
    GtkWidget *manual_holder;
    GtkWidget *manual_button;
    GtkWidget *black_and_white_button;
    GtkWidget *canny_button;
    GtkWidget *median_button;
    GtkWidget *threshold_button;
    GtkWidget *inversion_button;
    GtkWidget *automatic_text;
    GtkWidget *automatic_button;
    GtkWidget *pre_process_menu;
    SDL_Surface *image_surface;
} Widgets;

typedef struct Application
{
    State state;
    Widgets widgets;
} Application;


/* AUXILIARY FUNCTIONS */

void apply_css (GtkWidget *widget, GtkCssProvider *provider)
{
    GtkStyleContext *styleContext = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(styleContext, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surfaceRGB = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    if (surfaceRGB == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_FreeSurface(surface);
    return surfaceRGB;
}

/* SOLVER MENU */

void pre_process_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    if(app->state == START)
    {
        app->state = PRE_PROCESSING;

        //Modify the image !! TEST WITH GRAYSCALE
        //Modify the surface of the original image
        SDL_Surface *image_surface = app->widgets.image_surface;
        surface_to_grayscale(image_surface);
        IMG_SavePNG(image_surface, "image_grayscale.png");

        //Render the new image as a widget and replace the old one
        GdkPixbuf *buf = gdk_pixbuf_new_from_file_at_scale("image_grayscale.png", 900, 679, FALSE, NULL);

        GtkWidget *new_image = gtk_image_new_from_pixbuf(buf);
        gtk_widget_set_name(new_image, "sudoku_image");

        gtk_widget_destroy(app->widgets.image);
        gtk_box_pack_start(GTK_BOX(app->widgets.image_box), new_image, TRUE, TRUE, 0);
        app->widgets.image = new_image;

        // Load new CSS file
        GtkCssProvider *newCssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(newCssProvider, "css/pre_process_state.css", NULL);

        // Apply the styles
        apply_css(app->widgets.main_title, newCssProvider);
        apply_css(app->widgets.pre_process_button, newCssProvider);
        apply_css(app->widgets.image_rotation_button, newCssProvider);
        apply_css(app->widgets.construction_button, newCssProvider);
        apply_css(app->widgets.detection_button, newCssProvider);
        apply_css(app->widgets.ai_button, newCssProvider);
        apply_css(app->widgets.image, newCssProvider);

        gtk_widget_show_all(app->widgets.main_window);
    }
}


void rotation_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    if(app->state == PRE_PROCESSING)
    {
        app->state = ROTATION;

        // Load new CSS file
        GtkCssProvider *newCssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(newCssProvider, "css/rotation_state.css", NULL);

        // Apply the styles
        apply_css(app->widgets.main_title, newCssProvider);
        apply_css(app->widgets.pre_process_button, newCssProvider);
        apply_css(app->widgets.image_rotation_button, newCssProvider);
        apply_css(app->widgets.construction_button, newCssProvider);
        apply_css(app->widgets.detection_button, newCssProvider);
        apply_css(app->widgets.ai_button, newCssProvider);
    }
}

void detection_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    if(app->state == ROTATION)
    {
        app->state = DETECTION;

        // Load new CSS file
        GtkCssProvider *newCssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(newCssProvider, "css/detection_state.css", NULL);

        // Apply the styles
        apply_css(app->widgets.main_title, newCssProvider);
        apply_css(app->widgets.pre_process_button, newCssProvider);
        apply_css(app->widgets.image_rotation_button, newCssProvider);
        apply_css(app->widgets.construction_button, newCssProvider);
        apply_css(app->widgets.detection_button, newCssProvider);
        apply_css(app->widgets.ai_button, newCssProvider);
    }
}

void ai_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    if(app->state == DETECTION)
    {
        app->state = AI;

        // Load new CSS file
        GtkCssProvider *newCssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(newCssProvider, "css/ai_state.css", NULL);

        // Apply the styles
        apply_css(app->widgets.main_title, newCssProvider);
        apply_css(app->widgets.pre_process_button, newCssProvider);
        apply_css(app->widgets.image_rotation_button, newCssProvider);
        apply_css(app->widgets.construction_button, newCssProvider);
        apply_css(app->widgets.detection_button, newCssProvider);
        apply_css(app->widgets.ai_button, newCssProvider);
    }
}

void construction_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    if(app->state == AI)
    {
        app->state = CONSTRUCTION;

        // Load new CSS file
        GtkCssProvider *newCssProvider = gtk_css_provider_new();
        gtk_css_provider_load_from_path(newCssProvider, "css/construction_state.css", NULL);

        // Apply the styles
        apply_css(app->widgets.main_title, newCssProvider);
        apply_css(app->widgets.pre_process_button, newCssProvider);
        apply_css(app->widgets.image_rotation_button, newCssProvider);
        apply_css(app->widgets.construction_button, newCssProvider);
        apply_css(app->widgets.detection_button, newCssProvider);
        apply_css(app->widgets.ai_button, newCssProvider);
    }
}


void load_solver_window()
{
    gtk_init(NULL, NULL);

    // Load Glade file
    GtkBuilder *builder = gtk_builder_new_from_file("glade/solver_menu.glade");

    // Load CSS file
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "css/styles.css", NULL);

    // Get the widgets from the builder
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));

    GtkWidget *image_box = GTK_WIDGET(gtk_builder_get_object(builder, "image_box"));
    GtkWidget *main_title = GTK_WIDGET(gtk_builder_get_object(builder, "main_title"));

    GtkWidget *pre_process_button = GTK_WIDGET(gtk_builder_get_object(builder, "pre_process_button"));
    GtkWidget *image_rotation_button = GTK_WIDGET(gtk_builder_get_object(builder, "image_rotation_button"));
    GtkWidget *construction_button = GTK_WIDGET(gtk_builder_get_object(builder, "construction_button"));
    GtkWidget *detection_button = GTK_WIDGET(gtk_builder_get_object(builder, "detection_button"));
    GtkWidget *ai_button = GTK_WIDGET(gtk_builder_get_object(builder, "ai_button"));
    GtkWidget *save_button = GTK_WIDGET(gtk_builder_get_object(builder, "save_button"));

    // Connect the destroy signal to the window
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Initialize the image in the image box
    GdkPixbuf *buf = gdk_pixbuf_new_from_file_at_scale("sudoku_1.jpeg", 900, 679, FALSE, NULL);

    GtkWidget *image = gtk_image_new_from_pixbuf(buf);
    gtk_widget_set_name(image, "sudoku_image");
    gtk_box_pack_start(GTK_BOX(image_box), image, TRUE, TRUE, 0);

    //Create the SDL_Surface for the image
    SDL_Surface *image_surface = load_image("sudoku_1.jpeg");

    // Apply the styles
    apply_css(main_window, cssProvider);
    apply_css(image, cssProvider);
    apply_css(main_title, cssProvider);
    apply_css(pre_process_button, cssProvider);
    apply_css(image_rotation_button, cssProvider);
    apply_css(construction_button, cssProvider);
    apply_css(detection_button, cssProvider);
    apply_css(ai_button, cssProvider);
    apply_css(save_button, cssProvider);

    //Create the app structure
    Application app =
            {
                .state = START,

                .widgets =
                        {
                            .main_window = main_window,
                            .image = image,
                            .ai_button = ai_button,
                            .construction_button = construction_button,
                            .detection_button = detection_button,
                            .image_rotation_button = image_rotation_button,
                            .main_title = main_title,
                            .pre_process_button = pre_process_button,
                            .save_button = save_button,
                            .image_box = image_box,
                            .image_surface = image_surface
                        },
            };

    //Connect the buttons
    g_signal_connect(pre_process_button, "clicked", G_CALLBACK(pre_process_button_clicked), &app);
    g_signal_connect(image_rotation_button, "clicked", G_CALLBACK(rotation_button_clicked), &app);
    g_signal_connect(detection_button, "clicked", G_CALLBACK(detection_button_clicked), &app);
    g_signal_connect(ai_button, "clicked", G_CALLBACK(ai_button_clicked), &app);
    g_signal_connect(construction_button, "clicked", G_CALLBACK(construction_button_clicked), &app);

    // Show the main window
    apply_css(save_button, cssProvider);
    gtk_widget_show_all(main_window);

    // Run the GTK main loop
    gtk_main();
}

/* FUNCTIONALITIES MENU */

void func__pre_proccess_button_clicked(GtkWidget *widget, gpointer data)
{
    Application *app = data;

    // Load new CSS file
    GtkCssProvider *newCssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(newCssProvider, "css/pre_process_state.css", NULL);

    // Apply the styles
    apply_css(app->widgets.pre_process_button, newCssProvider);
    apply_css(app->widgets.image_rotation_button, newCssProvider);
    apply_css(app->widgets.detection_button, newCssProvider);
    apply_css(app->widgets.black_and_white_button, newCssProvider);
    apply_css(app->widgets.canny_button, newCssProvider);
    apply_css(app->widgets.median_button, newCssProvider);
    apply_css(app->widgets.threshold_button, newCssProvider);
    apply_css(app->widgets.inversion_button, newCssProvider);

    //Show the pre-process widget
    gtk_widget_show_all(app->widgets.pre_process_menu);
}

void load_func_window()
{
    gtk_init(NULL, NULL);

    // Load Glade file
    GtkBuilder *builder = gtk_builder_new_from_file("glade/functionalities_menu.glade");

    // Load CSS file
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "css/styles.css", NULL);

    // Get the widgets from the builder
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));

    GtkWidget *image_box = GTK_WIDGET(gtk_builder_get_object(builder, "image_box"));
    GtkWidget *button_box = GTK_WIDGET(gtk_builder_get_object(builder, "button_box"));

    GtkWidget *manual_text = GTK_WIDGET(gtk_builder_get_object(builder, "manual_text"));
    GtkWidget *manual_holder = GTK_WIDGET(gtk_builder_get_object(builder, "manual_holder"));
    GtkWidget *manual_button = GTK_WIDGET(gtk_builder_get_object(builder, "manual_button"));

    GtkWidget *automatic_text = GTK_WIDGET(gtk_builder_get_object(builder, "automatic_text"));
    GtkWidget *automatic_button = GTK_WIDGET(gtk_builder_get_object(builder, "automatic_button"));

    GtkWidget *pre_process_button = GTK_WIDGET(gtk_builder_get_object(builder, "fun_pre_process"));
    GtkWidget *image_rotation_button = GTK_WIDGET(gtk_builder_get_object(builder, "fun_rotation"));
    GtkWidget *detection_button = GTK_WIDGET(gtk_builder_get_object(builder, "fun_grid"));

    GtkWidget *black_and_white_button = GTK_WIDGET(gtk_builder_get_object(builder, "black_and_white"));
    GtkWidget *threshold_button = GTK_WIDGET(gtk_builder_get_object(builder, "threshold"));
    GtkWidget *median_button = GTK_WIDGET(gtk_builder_get_object(builder, "median"));
    GtkWidget *canny_button = GTK_WIDGET(gtk_builder_get_object(builder, "canny_edge"));
    GtkWidget *inversion_button = GTK_WIDGET(gtk_builder_get_object(builder, "inversion"));


    // Connect the destroy signal to the window
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Initialize the image in the image box
    GdkPixbuf *buf = gdk_pixbuf_new_from_file_at_scale("sudoku_1.jpeg", 900, 679, FALSE, NULL);

    GtkWidget *image = gtk_image_new_from_pixbuf(buf);
    gtk_widget_set_name(image, "sudoku_image");
    gtk_box_pack_start(GTK_BOX(image_box), image, TRUE, TRUE, 0);

    //Create the SDL_Surface for the image
    SDL_Surface *image_surface = load_image("sudoku_1.jpeg");

    // Apply the styles
    apply_css(main_window, cssProvider);
    apply_css(image, cssProvider);
    apply_css(pre_process_button, cssProvider);
    apply_css(image_rotation_button, cssProvider);
    apply_css(detection_button, cssProvider);

    //Create the app structure
    Application app =
            {
                    .state = START,

                    .widgets =
                            {
                                    .main_window = main_window,
                                    .image = image,
                                    .detection_button = detection_button,
                                    .image_rotation_button = image_rotation_button,
                                    .pre_process_button = pre_process_button,
                                    .image_box = image_box,
                                    .image_surface = image_surface,
                                    .manual_text = manual_text,
                                    .manual_holder = manual_holder,
                                    .manual_button = manual_button,
                                    .automatic_button = automatic_button,
                                    .automatic_text = automatic_text,
                                    .black_and_white_button = black_and_white_button,
                                    .canny_button = canny_button,
                                    .median_button = median_button,
                                    .threshold_button = threshold_button,
                                    .inversion_button = inversion_button,
                                    .pre_process_menu = button_box
                            },
            };

    //Connect the buttons
    g_signal_connect(pre_process_button, "clicked", G_CALLBACK(func__pre_proccess_button_clicked), &app);
    /*
    g_signal_connect(image_rotation_button, "clicked", G_CALLBACK(rotation_button_clicked), &app);
    g_signal_connect(detection_button, "clicked", G_CALLBACK(detection_button_clicked), &app);
    g_signal_connect(ai_button, "clicked", G_CALLBACK(ai_button_clicked), &app);
    g_signal_connect(construction_button, "clicked", G_CALLBACK(construction_button_clicked), &app);
     */

    // Show the main window
    gtk_widget_show_all(main_window);

    //Hide the widgets related to the hidden menus
    gtk_widget_hide(button_box);
    gtk_widget_hide(automatic_text);
    gtk_widget_hide(automatic_button);
    gtk_widget_hide(manual_text);
    gtk_widget_hide(manual_holder);
    gtk_widget_hide(manual_button);

    // Run the GTK main loop
    gtk_main();
}


/* MAIN MENU */

void solver_button_clicked(GtkWidget *widget, gpointer data)
{
    //Get the window from the arguments
    GtkWidget *main_window = GTK_WIDGET(data);

    //Destroy and quit the window
    gtk_widget_destroy(main_window);
    gtk_main_quit();

    load_solver_window();
}

void func_button_clicked(GtkWidget *widget, gpointer data)
{
    //Get the window from the arguments
    GtkWidget *main_window = GTK_WIDGET(data);

    //Destroy and quit the window
    gtk_widget_destroy(main_window);
    gtk_main_quit();

    load_func_window();
}

void load_main_window()
{
    gtk_init(NULL, NULL);

    // Load Glade file
    GtkBuilder *builder = gtk_builder_new_from_file("glade/main_menu.glade");

    // Load CSS file
    GtkCssProvider *cssProvider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(cssProvider, "css/styles.css", NULL);

    // Get the widgets from the builder
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed"));

    GtkWidget *ocr_text = GTK_WIDGET(gtk_builder_get_object(builder, "ocr_text"));
    GtkWidget *solver_button = GTK_WIDGET(gtk_builder_get_object(builder, "solver_button"));
    GtkWidget *func_button = GTK_WIDGET(gtk_builder_get_object(builder, "func_button"));
    GtkWidget *authors_text = GTK_WIDGET(gtk_builder_get_object(builder, "authors_text"));

    // Connect the destroy signal to the window
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    //Apply the styles
    apply_css(main_window, cssProvider);
    apply_css(authors_text, cssProvider);
    apply_css(ocr_text, cssProvider);
    apply_css(func_button, cssProvider);
    apply_css(solver_button, cssProvider);

    //Connect the buttons
    g_signal_connect(solver_button, "clicked", G_CALLBACK(solver_button_clicked), main_window);
    g_signal_connect(func_button, "clicked", G_CALLBACK(func_button_clicked), main_window);

    // Show the main window
    gtk_widget_show_all(main_window);

    // Run the GTK main loop
    gtk_main();
}


int main(int argc, char *argv[])
{
    load_main_window();
    return 0;
}