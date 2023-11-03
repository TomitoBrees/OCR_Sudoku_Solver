#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <err.h>
#include <string.h>

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

void filter_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *container = GTK_WIDGET(data);


    // Get a list of all child widgets in the container
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));

    // Iterate through the list of child widgets
    for (GList *node = children; node != NULL; node = node->next)
    {
        GtkWidget *child = GTK_WIDGET(node->data);

        // Check if the child is a button and not the "save" button
        if (GTK_IS_BUTTON(child) && strcmp(gtk_button_get_label(GTK_BUTTON(child)), "SAVE") != 0)
        {
            gtk_widget_hide(child);
        }

        if (GTK_IS_BUTTON(child))
        {
            if(strcmp(gtk_button_get_label(GTK_BUTTON(child)), "BACK") == 0 || strcmp(gtk_button_get_label(GTK_BUTTON(child)), "GRAYSCALE") == 0 ||
                strcmp(gtk_button_get_label(GTK_BUTTON(child)), "MEDIAN") == 0 || strcmp(gtk_button_get_label(GTK_BUTTON(child)), "MEDIUM") == 0 ||
                strcmp(gtk_button_get_label(GTK_BUTTON(child)), "SMOOTHING") == 0 || strcmp(gtk_button_get_label(GTK_BUTTON(child)), "BLACK/WHITE") == 0 ||
                    strcmp(gtk_button_get_label(GTK_BUTTON(child)), "ADAPTATIVE") == 0)
            {
                gtk_widget_show(child);
            }
        }
    }

    // Free the GList
    g_list_free(children);
}


void create_gtk_window(const char *image_file, SDL_Surface* surface)
{
    // INITIALISATION OF THE WINDOW
    int width = surface->w;
    int height = surface->h;

    printf("width = %i\nheight = %i\n", width, height);

    GdkRGBA black;

    black.red = 0;
    black.green = 0;
    black.blue = 0;
    black.alpha = 1.0;

    while (width > 1200 || height > 800)
    {
        width *= 0.9;
        height *= 0.9;
    }

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale(image_file, width, height, TRUE, NULL);


    printf("width = %i\nheight = %i\n", width, height);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *fixed = gtk_fixed_new();
    GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

    GtkWidget *rotate = gtk_button_new_with_label("⟳");
    GtkWidget *filter = gtk_button_new_with_label("FILTERS");
    GtkWidget *save = gtk_button_new_with_label("SAVE");

    gtk_window_set_title(GTK_WINDOW(window), "OCR");
    gtk_window_set_default_size(GTK_WINDOW(window), width + 50, height + 100);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_override_background_color(GTK_WIDGET(window), GTK_STATE_NORMAL, &black);

    gtk_container_add(GTK_CONTAINER(window), fixed);

    gtk_fixed_put(GTK_FIXED(fixed), image, 25 , 25);
    gtk_fixed_put(GTK_FIXED(fixed), rotate, (width / 2) - 30, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), filter, (width / 2) + 30, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), save, 25 , height + 50);


    gtk_widget_show_all(window);

    //BUTTONS FOR FILTER MENU
    GtkWidget *back = gtk_button_new_with_label("BACK");
    GtkWidget *grayscale = gtk_button_new_with_label("GRAYSCALE");
    GtkWidget *median = gtk_button_new_with_label("MEDIAN");
    GtkWidget *medium = gtk_button_new_with_label("MEDIUM");
    GtkWidget *smoothing = gtk_button_new_with_label("SMOOTHING");
    GtkWidget *blackwhite = gtk_button_new_with_label("BLACK/WHITE");
    GtkWidget *adaptative = gtk_button_new_with_label("ADAPTATIVE");

    gtk_fixed_put(GTK_FIXED(fixed), grayscale, (width / 2) + 25, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), median, (width / 2) - 75, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), medium, (width / 2) + 150, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), smoothing, (width / 2) - 205, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), blackwhite, (width / 2) + 250, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), adaptative, (width / 2) - 335, height + 50);
    gtk_fixed_put(GTK_FIXED(fixed), back, width - 50, height + 50);

    //BUTTON MANAGEMENT
    g_signal_connect(G_OBJECT(filter), "clicked", G_CALLBACK(filter_button_clicked), fixed);
}


int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    if (argc != 2) {
        g_print("Usage: %s <image_file>\n", argv[0]);
        return 1;
    }

    const char *image_file = argv[1];

    SDL_Surface* surface = load_image(image_file);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    create_gtk_window(image_file, surface);

    gtk_main();

    return 0;
}