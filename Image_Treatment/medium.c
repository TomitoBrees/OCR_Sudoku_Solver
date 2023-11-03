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

Uint32 pixel_to_mediumfilter(SDL_PixelFormat* format, SDL_Surface* surface,int i, int j)
{
        Uint8 R,G,B;
        Uint8 r,g,b;
        Uint32* pixels=surface->pixels;

        Uint32 actual=pixel_to_grayscale(pixels[i*surface->w+j],format);
        SDL_GetRGB(actual,format,&R,&G,&B);

        R=4*R/16;
        G=4*G/16;
        B=4*B/16;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;

        actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;

        actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;

        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;

        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;


        Uint32 color=SDL_MapRGB(format,R,G,B);

        return color;
}


void surface_to_mediumfilter(SDL_Surface* surface)
{
        Uint32* pixels=surface->pixels;

        SDL_PixelFormat* format=surface->format;

        SDL_LockSurface(surface);
        int width=surface->w;
        int height=surface->h;

        for(int i=1;i<height-1;i++)
        {
                for(int j=1;j<width-1;j++)
                {
                        pixels[i*width+j]=pixel_to_mediumfilter(format,surface,i,j);
                }
        }

        SDL_UnlockSurface(surface);
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
        surface_to_mediumfilter(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_medium.png");

        return EXIT_SUCCESS;
}
