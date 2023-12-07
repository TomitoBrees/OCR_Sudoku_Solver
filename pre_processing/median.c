#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Grayscale conversion
static Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;

    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

static Uint8 median(Uint8 l[])
{
        int i,j,k;
        for(i=1;i<9;i++)
        {
                k=l[i];
                j=i-1;
                while(j>=0 && l[j] > k)
                {
                        l[j+1]=l[j];
                        j=j-1;
                }
                l[j+1]=k;
        }
        return l[4];
}


static Uint32 pixel_to_medianfilter(SDL_PixelFormat* format, SDL_Surface* surface,int i, int j)
{
        Uint8 R[9];
        Uint8 G[9];
        Uint8 B[9];
        Uint8 r,g,b;
        Uint32* pixels=surface->pixels;

        //Uint32 actual=pixel_to_grayscale(pixels[i*surface->w+j],format);
        Uint32 actual=pixels[i*surface->w+j];
	SDL_GetRGB(actual,format,&r,&g,&b);
        R[0]=r;
        G[0]=g;
        B[0]=b;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[1]=r;
        G[1]=g;
        B[1]=b;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[2]=r;
        G[2]=g;
        B[2]=b;

        actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[3]=r;
        G[3]=g;
        B[3]=b;

        actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[4]=r;
        G[4]=g;
        B[4]=b;

        actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[5]=r;
        G[5]=g;
        B[5]=b;
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[6]=r;
        G[6]=g;
        B[6]=b;

        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[7]=r;
        G[7]=g;
        B[7]=b;

        actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[8]=r;
        G[8]=g;
        B[8]=b;

        Uint32 color=SDL_MapRGB(format,median(R),median(G),median(B));

        return color;
}


void surface_to_medianfilter(SDL_Surface* surface)
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
                        pixels[i*width+j]=pixel_to_medianfilter(format,surface,i,j);
                }
        }

        SDL_UnlockSurface(surface);
}

