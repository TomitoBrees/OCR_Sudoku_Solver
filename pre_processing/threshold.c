#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Uint32 pixel_to_adaptive_threshold(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height)
{
        int block_size=3;
        int half_block=block_size/2;
        int seuil_crit=55;
        int count=0;

        Uint8 R=0;
	Uint8 G=0;
	Uint8 B=0;

        Uint8 r,g,b;

        Uint32 actual;

        Uint32* pixels=surface->pixels;

        for(int x=-half_block;x<=half_block;x++)
        {
                for(int y=-half_block;y<=half_block;y++)
                {
                        if(i+x >= 0 && i+x < height && j+y >= 0 && j+y <width)
                        {
                                actual=pixels[(i+x)*width+j+y];
                                SDL_GetRGB(actual,format,&r,&g,&b);
                                R+=r;
                                G+=g;
                                B+=b;
                                count++;
                        }

                }
        }

        int average_R=R/count;
        int average_G=G/count;
        int average_B=B/count;
        actual=pixels[i*width+j];
        Uint8 actual_r,actual_g,actual_b;
        SDL_GetRGB(actual,format,&actual_r,&actual_g,&actual_b);
        int average=(actual_r+actual_g+actual_b)/3;
        int average_all=(average_R+average_G+average_B)/3;

        Uint32 color;

        if(average-average_all>seuil_crit)
        {
                color=SDL_MapRGB(format,255,255,255);
        }
        else
                color=SDL_MapRGB(format,0,0,0);
        return color;
}

void surface_to_threshold_filter(SDL_Surface* surface)
{
        Uint32* pixels=surface->pixels;

        SDL_Surface* copie=surface;

        SDL_PixelFormat* format=surface->format;

        SDL_LockSurface(surface);

        int width=surface->w;
        int height=surface->h;

        for(int i=1;i<height-1;i++)
        {
                for(int j=1;j<width-1;j++)
                {
                        pixels[i*width+j]=pixel_to_adaptive_threshold(format,copie,i,j,width,height);
                }
        }

        SDL_UnlockSurface(surface);
}

