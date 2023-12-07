#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

Uint32* integral_image(SDL_Surface* surface)
{
	Uint32* integral =malloc(surface->w*surface->h * sizeof(Uint32));
	if(!integral)
	{
		errx(EXIT_FAILURE,"error during integral");	
	}

	SDL_LockSurface(surface);

	for(int y=0;y<surface->h;y++)
	{
		Uint32 sum=0;
		for(int x=0;x<surface->w;x++)
		{
			Uint32 pixel =((Uint32*)surface->pixels)[y*surface->w+x];
			Uint8 r,g,b;
			SDL_GetRGB(pixel,surface->format,&r,&g,&b);
			Uint8 average=(r+g+b)/3;

			sum+=average;
			if(y==0)
			{
				integral[y*surface->w+x]=sum;
			}
			else
			{
				integral[y*surface->w+x]=integral[(y-1)*surface->w+x]+sum;
			}
		}
	}
	SDL_UnlockSurface(surface);
	return integral;
}

Uint8 fast_local_threshold(Uint32* surface,int x, int y, int width, int height, int neighbor)
{
	int side=neighbor/2;

	int x1=x-side-1;
	int y1=y-side-1;
	int x2=x+side;
	int y2=y+side;


	x1=(x1<0) ? 0 : x1;
	y1=(y1<0) ? 0 : y1;
	x2=(x2>=width) ? width -1 : x2;
	y2=(y2>=height) ? height -1 : y2;

	int count=(x2-x1)*(y2-y1);
	//printf("%i\n",count);
	//int count = (abs(x2 - x1) + 1) * (abs(y2 - y1) + 1);

	Uint32 sum=surface[y2*width+x2] - surface[y1*width+x2] - surface[y2*width+x1] + surface[y1*width+x1];

	return (Uint8)(sum/count);
}



void surface_to_adaptative_threshold2(SDL_Surface* surface) {
	int neighborhood_size = 21; // Ensure this is an odd number

	SDL_Surface* copy = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
	if (!copy) {
		errx(EXIT_FAILURE, "Error during copying");
	}

	Uint32* integral = integral_image(copy);
	if (!integral) {
		SDL_FreeSurface(copy);
		errx(EXIT_FAILURE, "Error calculating integral image");
	}

	Uint8 local_threshold;
	SDL_LockSurface(surface);

	for (int y = 0; y < surface->h; y++) {
		for (int x = 0; x < surface->w; x++) {
			local_threshold = fast_local_threshold(integral, x, y, surface->w, surface->h, neighborhood_size);
			Uint32 pixel = ((Uint32*)surface->pixels)[y * surface->w + x];
			Uint8 r, g, b;
			SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			Uint8 average = (r + g + b) / 3;

			if (average < local_threshold) {
				((Uint32*)surface->pixels)[y * surface->w + x] = SDL_MapRGB(surface->format, 0, 0, 0);
			} else {
				((Uint32*)surface->pixels)[y * surface->w + x] = SDL_MapRGB(surface->format, 255, 255, 255);
			}
		}
	}

	SDL_UnlockSurface(surface);
	SDL_FreeSurface(copy);
	free(integral);
}
