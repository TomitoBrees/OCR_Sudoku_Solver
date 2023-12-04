#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_BINS 65536 // 16-bit image has 2^16 levels

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


// Calculate histogram of the 64-bit grayscale image
void calculate_histogram_64(SDL_Surface* surface, long* histogram) {
    Uint16* pixels = (Uint16*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            Uint16 pixel = pixels[i * width + j];
            histogram[pixel]++;
        }
    }
}

// Calculate Otsu's threshold value for a 64-bit grayscale image
int otsu_threshold_64(SDL_Surface* surface) {
    long histogram[NUM_BINS] = {0}; // Assuming 16-bit grayscale image
    calculate_histogram_64(surface, histogram);

    long total = (long)surface->w * (long)surface->h;
    double sum = 0;

    for (int i = 0; i < NUM_BINS; i++) {
        sum += i * histogram[i];
    }

    double sumB = 0;
    long wB = 0;
    long wF;
    double varMax = 0;
    int threshold = 0;

    for (int i = 0; i < NUM_BINS; i++) {
        wB += histogram[i];
        if (wB == 0) continue;
        wF = total - wB;

        if (wF == 0) break;

        sumB += (double)(i * histogram[i]);

        double meanB = sumB / wB;
        double meanF = (sum - sumB) / wF;

        double varBetween = (double)wB * (double)wF * (meanB - meanF) * (meanB - meanF);

        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
        }
    }

    return threshold;
}



Uint32 pixel_to_adaptive_threshold(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height,int seuil)
{
        int block_size=3;
        int half_block=block_size/2;
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

        if(average-average_all>seuil)
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

	//int seuil=otsu_threshold_64(surface);
	int seuil=seuil_calcul(format,surface,width,height,200);
        for(int i=1;i<height-1;i++)
        {
                for(int j=1;j<width-1;j++)
                {
                        pixels[i*width+j]=pixel_to_adaptive_threshold(format,copie,i,j,width,height,seuil);
                }
        }

        SDL_UnlockSurface(surface);
}

