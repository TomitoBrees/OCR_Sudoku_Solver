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

//Still in process
int seuil_calcul(SDL_PixelFormat* format,SDL_Surface* surface,int width,int height,double crit)
{
        Uint8 sum_R;
        Uint8 sum_G;
        Uint8 sum_B;

        Uint8 sum_square_R;
        Uint8 sum_square_G;
        Uint8 sum_square_B;

        Uint8 r;
        Uint8 g;
        Uint8 b;

        Uint32* pixels=surface->pixels;

        int pixel_count=width*height;

        for(int i=0; i<pixel_count;i++)
        {
                SDL_GetRGB(pixels[i],format,&r,&g,&b);
                sum_R+=r;
                sum_G+=g;
                sum_B+=b;
                sum_square_R+=r*r;
                sum_square_G+=g*g;
                sum_square_B+=b*b;
        }

        double mean_R=(double)(sum_R/pixel_count);
        double mean_G=(double)(sum_G/pixel_count);
        double mean_B=(double)(sum_B/pixel_count);

        double stdDev_R=(double)sqrt(((double)(sum_square_R)-(double)sum_R*(double)sum_R/pixel_count)/(pixel_count-1));
        double stdDev_G=(double)sqrt(((double)(sum_square_G)-(double)sum_G*(double)sum_G/pixel_count)/(pixel_count-1));
        double stdDev_B=(double)sqrt(((double)(sum_square_B)-(double)sum_B*(double)sum_B/pixel_count)/(pixel_count-1));

        return (int)((mean_R+mean_G+mean_B)/3+ crit*(stdDev_R+stdDev_G+stdDev_B)/3);
}


Uint32 pixel_to_adaptive_threshold(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height)
{
        int block_size=3;
        int half_block=block_size/2;
        int seuil_crit=seuil_calcul(format,surface,width,height,0.5);
        int count=0;

        Uint8 R,G,B;

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
        surface_to_threshold_filter(surface);

        //Save greyscale image
        IMG_SavePNG(surface, "test_threshold.png");

        return EXIT_SUCCESS;
}
