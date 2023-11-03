#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>


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

//Rotate the image to 90 degrees
SDL_Surface* rotation90(SDL_Surface* surface)
{
    Uint32 *pixels = surface->pixels;
    int width = surface->w;
    int height = surface->h;
    SDL_PixelFormat *format = surface->format;
    int depth = format->BitsPerPixel;

    SDL_Surface* rotated = SDL_CreateRGBSurface(0, height, width, depth, 0, 0, 0, 0);
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

        SDL_Surface* rotated = SDL_CreateRGBSurface(0, new_width, new_height, depth, 0, 0, 0, 0);
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

int main(int argc, char** argv)
{
    //Checks the number of arguments.
    if (argc != 3)
        errx(EXIT_FAILURE, "Usage: image-file");

    int degree = 0;
    int i = 0;
    while (argv[2][i] != 0)
    {
        degree += argv[2][i] - 48;
        degree *= 10;
        i++;
    }

    degree /= 10;

    //Create a surface from the colored image.
    SDL_Surface* surface = load_image(argv[1]);
    if (surface == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* rotated = rotation(surface, degree);
    if (rotated == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    IMG_SavePNG(rotated, "test_rotate.png");

    return EXIT_SUCCESS;
}