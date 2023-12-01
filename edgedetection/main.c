#include "stdlib.h"
#include "stdio.h"
#include "err.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

#include "include/edge_detection.h"

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL, NULL);
    SDL_RenderPresent(renderer);
}

/*
void event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* cannyTexture)
{
    SDL_Event event;
    SDL_Texture* t = colored;
    draw(renderer, t);

    while (1)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            // If the "quit" button is pushed, ends the event loop.
            case SDL_QUIT:
                return;
            // If the window is resized, updates and redraws the diagonals.
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    draw(renderer, t);
                }
                break;
            case SDL_KEYDOWN:
                if (t == colored)
                {
                    t = cannyTexture;
                    draw(renderer, t);
                }
                else
                {
                    t = colored;
                    draw(renderer, t);
                }
                break;
        }
    }
}*/

void event_loop(SDL_Renderer* renderer, SDL_Texture** textures, size_t text_len)
{
    SDL_Event event;
    size_t c = 0;
    draw(renderer, textures[c]);
    while (1) {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                return;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    draw(renderer, textures[c]);
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        c = (c+(text_len-1)) % text_len;
                        break;
                    case SDLK_RIGHT:
                        c = (c+1) % text_len;
                        break;
                }
                draw(renderer, textures[c]);
        }
    }
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface)
        errx(EXIT_FAILURE, "INIT SURFACE ERROR: %s", IMG_GetError());

    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface);
    if (!new_surface)
        errx(EXIT_FAILURE, "RGB SURFACE ERROR: %s", SDL_GetError());

    return new_surface;
}

SDL_Texture ** test(SDL_Renderer *renderer, SDL_Surface *image, size_t *count) {
    SDL_Surface *original = SDL_ConvertSurface(image, image->format, 0);

    int sobelX[3][3] = { { -1, 0, 1 }, 
                         { -2, 0, 2 }, 
                         { -1, 0, 1 } }; // Sobel Matrix for X convolution

    int sobelY[3][3] = { { -1, -2, -1 },
                         {  0,  0,  0 }, 
                         {  1,  2,  1 } }; // Sobel Matrix for Y convolution

    int height = image->h;
    int width = image->w;

    Uint32 magnitudes[width*height]; // Gradient obtained with Sobel

    Uint32 angles[width*height]; // Angles used for non maxima removal

    *count = 6;
    SDL_Texture **textures = malloc((*count) * sizeof(SDL_Texture *));

    size_t c = 0;
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    //edgeDetection(image, original);
    //textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    applySobelConvolution(height, width, image->pixels, image->format,
            sobelX, sobelY, magnitudes, angles);
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    nonMaximaSuppression(height, width, image->pixels, magnitudes, angles);
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    applyThresholding(height, width, image->pixels,
            magnitudes, image->format);
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    hysteresisAnalysis(height, width, image->pixels, magnitudes);
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    houghTransform(height, width, image->pixels, image->format);
    textures[c++] = SDL_CreateTextureFromSurface(renderer, image);

    return textures;
}

int main(int argc, char** argv)
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window = SDL_CreateWindow("Hough Transform", 0, 0, 400,400,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(EXIT_FAILURE, "%s", SDL_GetError());


    SDL_Surface* surface = load_image(argv[1]);
    //SDL_Surface* original_surface = load_image(argv[1]);

    SDL_SetWindowSize(window,surface->w,surface->h);

    size_t count;
    SDL_Texture **textures = test(renderer, surface, &count);

    event_loop(renderer, textures, count);

    for (size_t i = 0; i < count; i++)
        SDL_DestroyTexture(textures[i]);
    free(textures);

    /*SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);

    edgeDetection(surface,original_surface);

    SDL_Texture* cannyTexture = SDL_CreateTextureFromSurface(renderer,surface);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(original_surface);

    event_loop(renderer,texture,cannyTexture);

    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(cannyTexture);*/

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

