#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


// Loads an image in a surface.
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

Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;

    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}


Uint32 pixel_to_smoothing_filter(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width)
{
        Uint32* pixels=surface->pixels;

        //int Seuil_fort=100;
        //int Seuil_bas=85;

        Uint32 actual=pixels[i*width+j];
        Uint8 R,G,B;
        SDL_GetRGB(actual,format,&R,&G,&B);
        if(R==255)
        {
                return SDL_MapRGB(format,255,255,255);
        }

        Uint8 r,g,b;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        if(r==0)
                return SDL_MapRGB(format,0,0,0);


        return SDL_MapRGB(format,255,255,255);
}


//only use this filter when the image is only black and white (after the seuil filter) NOT FINISHED
void surface_to_smoothing_filter(SDL_Surface* surface)
{
        Uint32* input_pixels=surface->pixels;

        SDL_Surface* copie=surface;

        SDL_PixelFormat* format=surface->format;

        SDL_LockSurface(surface);
        SDL_LockSurface(copie);

        int width=surface->w;
        int height=surface->h;

        for(int i=1;i<height-1;i++)
        {
                for(int j=1;j<width-1;j++)
                {
                        input_pixels[i*width+j]=pixel_to_smoothing_filter(format,copie,i,j,width);
                }
        }

        SDL_UnlockSurface(surface);
        SDL_UnlockSurface(copie);
}


int main(int argc, char** argv)
{
        //Checks the number of arguments.
        if (argc != 2)
                errx(EXIT_FAILURE, "Usage: image-file");

        //Create a surface from the colored image.
        SDL_Surface* surface = load_image(argv[1]);
        if (surface == NULL)
        {
            errx(EXIT_FAILURE, "%s", SDL_GetError());

        }
        //Convert the surface into greyscale
        surface_to_smoothing_filter(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_smoothing.png");

        return EXIT_SUCCESS;
}
