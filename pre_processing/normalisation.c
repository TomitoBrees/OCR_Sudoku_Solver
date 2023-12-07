#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Uint32 pixel_to_normalisation(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,Uint8 max_R,Uint8 max_G,Uint8 max_B)
{
	Uint32* pixels=surface->pixels;
	Uint8 r;
	Uint8 g;
	Uint8 b;

	SDL_GetRGB(pixels[i*width+j],format,&r,&g,&b);

	r=(r)*(255/max_R);
	g=(g)*(255/max_G);
	b=(b)*(255/max_B);

	return SDL_MapRGB(format,r,g,b);
}

void max_in_img(SDL_Surface* surface,SDL_PixelFormat* format,int width,int height,Uint8* R,Uint8* G,Uint8* B)
{
	Uint32* pixels=surface->pixels;
	Uint8 max_R=0;
	Uint8 max_G=0;
	Uint8 max_B=0;

	Uint8 r;
	Uint8 g;
	Uint8 b;

	for(int i=0;i<height-1;i++)
	{
		for(int j=0;j<width-1;j++)
		{
			SDL_GetRGB(pixels[i*width+j],format,&r,&g,&b);
			if(r>max_R)
				max_R=r;
			if(g>max_G)
				max_G=g;
			if(b>max_B)
				max_B=b;
		}
	}
	*R=max_R;
	*G=max_G;
	*B=max_B;
}

void surface_to_normalisation_filter(SDL_Surface* surface)
{
	int hist[256] = {0}; // Initialize histogram with zeros for each intensity level
	int cdf[256] = {0};  // Initialize cumulative distribution function

	Uint32* pixels=surface->pixels;

	SDL_Surface* copie=surface;

	SDL_PixelFormat* format=surface->format;

	SDL_LockSurface(surface);

	int width=surface->w;
	int height=surface->h;
	Uint8 max_img_R;
	Uint8 max_img_G;
	Uint8 max_img_B;

	int len = surface->w * surface->h;

	// Calculate histogram
	for (int i = 0; i < len; ++i) 
	{
		Uint8 gray;
		SDL_GetRGB(pixels[i], surface->format, &gray, &gray, &gray);
		hist[gray]++;
	}

	// Calculate cumulative distribution function
	int sum = 0;
	for (int i = 0; i < 256; ++i) 
	{
		sum += hist[i];
		cdf[i] = sum * 255 / len;
	}

	max_in_img(surface,format,width,height,&max_img_R,&max_img_G,&max_img_B);

	for (int i = 0; i < len; ++i) 
	{
        	Uint8 gray;
        	SDL_GetRGB(pixels[i], surface->format, &gray, &gray, &gray);
        	gray = cdf[gray];
        	pixels[i] = SDL_MapRGB(surface->format, gray, gray, gray);
    	}
	for(int i=1;i<height-1;i++)
	{
		for(int j=1;j<width-1;j++)
		{
			pixels[i*width+j]=pixel_to_normalisation(format,copie,i,j,width,max_img_R,max_img_G,max_img_B);
		}
	}

	SDL_UnlockSurface(surface);
}

