#include <err.h>
#include <SDL2/SDL.h>
//#include <math.h>

//Rotate the image to 90 degrees
SDL_Surface* rotation90(SDL_Surface* surface)
{
    Uint32 *pixels = surface->pixels;
    int width = surface->w;
    int height = surface->h;
    SDL_PixelFormat *format = surface->format;
    int depth = format->BitsPerPixel;

    SDL_Surface* rotated = SDL_CreateRGBSurface(0, height, width, depth, format->Rmask, format->Gmask, format->Bmask, format->Amask);
    if (rotated == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    Uint32 *rotatedpixel = rotated->pixels;

    SDL_LockSurface(surface);
    SDL_LockSurface(rotated);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            rotatedpixel[j * height + (height - 1 - i)] = pixels[i * width + j];
        }
    }

    SDL_UnlockSurface(surface);
    SDL_UnlockSurface(rotated);
    return rotated;
}

//Rotates the image to any angle, this is the function called in main.
SDL_Surface* rotation(SDL_Surface* surface, int degree)
{
    if(degree % 90 == 0)
    {
        int iteration = degree / 90;
        SDL_Surface* new = rotation90(surface);

        for (int i = 0; i < iteration - 1; ++i)
        {
            new = rotation90(new);
        }

        return new;
    }
    else
    {
        int width = surface->w;
        int height = surface->h;
        SDL_PixelFormat *format = surface->format;
        int depth = format->BitsPerPixel;

        double radian = -degree * M_PI / 180;
        double s = sin(radian);
        double c = cos(radian);

        int new_width = (int)(fabs(width * c) + fabs(height * s));
        int new_height = (int)(fabs(width * s) + fabs(height * c));

        SDL_Surface* rotated = SDL_CreateRGBSurface(0, new_width, new_height, depth, format->Rmask, format->Gmask, format->Bmask, format->Amask);
        if (rotated == NULL)
            errx(EXIT_FAILURE, "%s", SDL_GetError());


        int centerX = width / 2;
        int centerY = height / 2;

        for (int x = 0; x < new_width; x++)
        {
            for (int y = 0; y < new_height; y++)
            {
                int sourceX = (int)((x - new_width / 2) * c - (y - new_height / 2) * s + centerX);
                int sourceY = (int)((x - new_width / 2) * s + (y - new_height / 2) * c + centerY);

                if (sourceX >= 0 && sourceX < width && sourceY >= 0 && sourceY < height)
                {
                    Uint32 pixel = ((Uint32*)surface->pixels)[sourceY * width + sourceX];
                    ((Uint32*)rotated->pixels)[y * new_width + x] = pixel;
                }
            }
        }

        return rotated;
    }
}

