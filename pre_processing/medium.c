#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Grayscale conversion
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;

    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

Uint32 pixel_to_mediumfilter(SDL_PixelFormat* format, SDL_Surface* surface,int i, int j)
{
        Uint8 R,G,B;
        Uint8 r,g,b;
        Uint32* pixels=surface->pixels;

        //Uint32 actual=pixel_to_grayscale(pixels[i*surface->w+j],format);
        Uint32 actual=pixels[i*surface->w+j];
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

