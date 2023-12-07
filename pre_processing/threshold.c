#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*int calculate_intensity_threshold(SDL_Surface* surface) {
	int histogram[256] = {0}; // Assuming 8-bit grayscale image

	// Calculate histogram
	Uint8* pixels = (Uint8*)surface->pixels;
	int width = surface->w;
	int height = surface->h;
	int totalPixels = width * height;

	for (int i = 0; i < totalPixels; i++) {
		histogram[pixels[i]]++;
	}

	// Calculate probabilities
	double probabilities[256];
	for (int i = 0; i < 256; i++) {
		probabilities[i] = (double)histogram[i] / totalPixels;
	}

	// Calculate cumulative probabilities and means
	double cumulativeProbabilities[256];
	double cumulativeMeans[256];
	cumulativeProbabilities[0] = probabilities[0];
	cumulativeMeans[0] = 0;

	for (int i = 1; i < 256; i++) {
		cumulativeProbabilities[i] = cumulativeProbabilities[i - 1] + probabilities[i];
		cumulativeMeans[i] = cumulativeMeans[i - 1] + i * probabilities[i];
	}

	// Calculate between-class variance
	double maxBetweenClassVariance = 0.0;
	int optimalThreshold = 0;

	for (int i = 0; i < 256; i++) {
		double betweenClassVariance = pow(cumulativeMeans[255] * cumulativeProbabilities[i] - cumulativeMeans[i], 2) /
			(cumulativeProbabilities[i] * (1 - cumulativeProbabilities[i]));

		if (betweenClassVariance > maxBetweenClassVariance) {
			maxBetweenClassVariance = betweenClassVariance;
			optimalThreshold = i;
		}
	}

	return optimalThreshold;
}*/

int intensity(SDL_Surface* surface,SDL_PixelFormat* format,int i,int j,int width,int height)
{
	int block_size=6;
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
	return (average_R+average_G+average_B)/3;
}

Uint8 threshold(SDL_Surface* surface,int x, int y, int width, int height, int neighbor)
{
	int side=neighbor/2;
	
	Uint32* pixels=surface->pixels;
	
	int x1=x-side-1;
	int y1=y-side-1;
	int x2=x+side;
	int y2=y+side;

	x1=(x1<0) ? 0 : x1;
	y1=(y1<0) ? 0 : y1;
	x2=(x2>=width) ? width -1 : x2;
	y2=(y2>=height) ? height -1 : y2;

	int count=(x2-x1)*(y2-y1);
	if(count>0)
	{
		Uint32 sum=pixels[y2*width+x2] - pixels[y1*width+x2] - pixels[y2*width+x1] + pixels[y1*width+x1];
	
		return (Uint8)(sum/count);
	}
	return 0;	
}


Uint32 pixel_to_adaptive_threshold(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height)
{
	int block_size=20;
	int half_block=block_size/2;
	int count=0;
	//Uint8 seuil=intensity(surface,format,i,j,width,height);
	Uint8 seuil=55;
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

	int seuil=55;//calculate_intensity_threshold(surface);
	for(int i=1;i<height-1;i++)
	{
		for(int j=1;j<width-1;j++)
		{
			pixels[i*width+j]=pixel_to_adaptive_threshold(format,copie,i,j,width,height);
		}
	}

	SDL_UnlockSurface(surface);
}

