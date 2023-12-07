#include "SDL2/SDL.h"
//#include "SDL2/SDL_image.h"

void applySobelConvolution(int height, int width, Uint32* pixels,SDL_PixelFormat* format,
                           int sobelX[3][3], int sobelY[3][3],
                           Uint32 magnitudes[width*height], Uint32 angles[width*height]);

void nonMaximaSuppression(int height, int width, Uint32* pixels,
                          Uint32 magnitudes[width*height], Uint32 angles[width*height]);

void applyThresholding(int height, int width, Uint32* pixels,
                          Uint32 magnitudes[width*height],
                          SDL_PixelFormat* format);

void hysteresisAnalysis(int height, int width, Uint32* pixels,
                          Uint32 magnitudes[width*height]);

void houghTransform(int height, int width, Uint32* pixels,SDL_PixelFormat* format);

void lineremoval(SDL_Surface* image, int width, int height,
                 SDL_PixelFormat* format, Uint32* pixels,
                 int i, int j);

void linedetection(SDL_Surface* image, int width, int height,
                               SDL_PixelFormat* format, Uint32* pixels);

void findAndExtractGridSquares(SDL_Surface* original, int width, int height,
                               SDL_PixelFormat* format, Uint32* pixels);

SDL_Surface* HoughDetection(SDL_Surface* image);


SDL_Surface *  automaticRotation(SDL_Surface* image);


SDL_Surface *  cannyEdgeDetection(SDL_Surface *image);
