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


Uint32 pixel_to_contrast(Uint32 pixel, SDL_PixelFormat* format,double factor)
{
	Uint8 R,G,B;
	SDL_GetRGB(pixel,format,&R,&G,&B);
	int new_R=(int)((R-128)*factor)+128;
	int new_G=(int)((G-128)*factor)+128;
	int new_B=(int)((B-128)*factor)+128;
	
	new_R=(new_R<0)? 0: (new_R>255)?255:new_R;
	new_G=(new_G<0)? 0: (new_G>255)?255:new_G;
	new_B=(new_B<0)? 0: (new_B>255)?255:new_B;

	return SDL_MapRGB(format,new_R,new_G,new_B);
}

void surface_to_contrast(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
	double factor=2;

    SDL_LockSurface(surface);

    for (int i = 0; i < len; ++i)
    {
        pixels[i] = pixel_to_contrast(pixels[i], format, factor);
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
        surface_to_contrast(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_contrast.png");

        return EXIT_SUCCESS;
}

