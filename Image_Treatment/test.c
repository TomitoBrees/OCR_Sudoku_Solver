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

//Greyscale conversion
Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    Uint8 average = 0.3 * r + 0.59 * g + 0.11 * b;

    Uint32 color = SDL_MapRGB(format, average, average, average);
    return color;
}

void surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);

    for (int i = 0; i < len; ++i)
    {
        pixels[i] = pixel_to_grayscale(pixels[i], format);
    }

    SDL_UnlockSurface(surface);
}


//Black & White conversion
Uint32 pixel_to_blackandwhite(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;

    SDL_GetRGB(pixel_color, format, &r, &g, &b);

    if((r + g + b)/3 > 155)
    {
        Uint32 color = SDL_MapRGB(format, 255, 255, 255);
        return color;
    }
    else
    {
        Uint32 color = SDL_MapRGB(format, 0, 0, 0);
        return color;
    }
}

void surface_to_blackandwhite(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;

    SDL_LockSurface(surface);

    for (int i = 0; i < len; ++i)
    {
        pixels[i] = pixel_to_blackandwhite(pixels[i], format);
    }

    SDL_UnlockSurface(surface);
}


Uint32 pixel_to_mediumfilter(SDL_PixelFormat* format, SDL_Surface* surface,int i, int j)
{
	Uint8 R,G,B;
	Uint8 r,g,b;
	Uint32* pixels=surface->pixels;

	Uint32 actual=pixel_to_grayscale(pixels[i*surface->w+j],format);
	SDL_GetRGB(actual,format,&R,&G,&B);
	
	R=4*R/16;
	G=4*G/16;
	B=4*B/16;

	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
	SDL_GetRGB(actual,format,&r,&g,&b);
	R+=r/16;
	G+=g/16;
	B+=b/16;
	
	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;
	
	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;
	
	actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;
	
	actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=2*r/16;
        G+=2*g/16;
        B+=2*b/16;
	
	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R+=r/16;
        G+=g/16;
        B+=b/16;
	
	
	Uint32 color=SDL_MapRGB(format,R,G,B);
	
	return color;
}

void surface_to_mediumfilter(SDL_Surface* surface)
{
	Uint32* pixels=surface->pixels;
	
	SDL_PixelFormat* format=surface->format;

	SDL_LockSurface(surface);
	int width=surface->w;
	int height=surface->h;

	for(int i=1;i<height-1;i++)
	{
		for(int j=1;j<width-1;j++)
		{
			pixels[i*width+j]=pixel_to_mediumfilter(format,surface,i,j);
		}
	}

	SDL_UnlockSurface(surface);
}


Uint8 median(Uint8 l[])
{
	int i,j,k;
	for(i=1;i<9;i++)
	{
		k=l[i];
		j=i-1;
		while(j>=0 && l[j] > k)
		{
			l[j+1]=l[j];
			j=j-1;
		}
		l[j+1]=k;
	}
	return l[4];
}


Uint32 pixel_to_medianfilter(SDL_PixelFormat* format, SDL_Surface* surface,int i, int j)
{
	Uint8 R[9];
	Uint8 G[9];
	Uint8 B[9];
	Uint8 r,g,b;
	Uint32* pixels=surface->pixels;
	
	Uint32 actual=pixel_to_grayscale(pixels[i*surface->w+j],format);
	SDL_GetRGB(actual,format,&r,&g,&b);
	R[0]=r;
	G[0]=g;
	B[0]=b;

	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
	SDL_GetRGB(actual,format,&r,&g,&b);
	R[1]=r;
        G[1]=g;
        B[1]=b;

	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[2]=r;
        G[2]=g;
        B[2]=b;

	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[3]=r;
        G[3]=g;
        B[3]=b;

	actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[4]=r;
        G[4]=g;
        B[4]=b;

	actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[5]=r;
        G[5]=g;
        B[5]=b;

	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[6]=r;
        G[6]=g;
        B[6]=b;

	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[7]=r;
        G[7]=g;
        B[7]=b;

	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
        R[8]=r;
        G[8]=g;
        B[8]=b;

	Uint32 color=SDL_MapRGB(format,median(R),median(G),median(B));

	return color;
}

void surface_to_medianfilter(SDL_Surface* surface)
{
        Uint32* pixels=surface->pixels;

        SDL_PixelFormat* format=surface->format;

        SDL_LockSurface(surface);
        
	int width=surface->w;
        int height=surface->h;

        for(int i=1;i<height-1;i++)
        {
                for(int j=1;j<width-1;j++)
                {
                        pixels[i*width+j]=pixel_to_medianfilter(format,surface,i,j);
                }
        }

        SDL_UnlockSurface(surface);
}


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

Uint32 pixel_to_seuil_adaptatif(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height)
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




void surface_to_seuilfilter(SDL_Surface* surface)
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
			pixels[i*width+j]=pixel_to_seuil_adaptatif(format,copie,i,j,width,height);
		}
	}

	SDL_UnlockSurface(surface);
}


Uint32 pixel_to_lissage_filter(SDL_PixelFormat* format,SDL_Surface* surface,int i,int j,int width,int height)
{
        Uint32* pixels=surface->pixels;
	
	int Seuil_fort=100;
	int Seuil_bas=85;

        Uint32 actual=pixels[i*width+j];
	Uint8 R,G,B;
	SDL_GetRGB(actual,format,&R,&G,&B);
	if(R==255)
	{
		return SDL_MapRGB(format,255,255,255);
	}

	Uint8 r,g,b;

	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j-1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
		return SDL_MapRGB(format,0,0,0);

	
     	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
                return SDL_MapRGB(format,0,0,0);

        
	actual=pixel_to_grayscale(pixels[(i-1)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
  	        return SDL_MapRGB(format,0,0,0);


     	actual=pixel_to_grayscale(pixels[(i)*surface->w+j-1],format);
       	SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
               	return SDL_MapRGB(format,0,0,0);

        
	actual=pixel_to_grayscale(pixels[(i)*surface->w+j+1],format);
        SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
                return SDL_MapRGB(format,0,0,0);


       	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j-1],format);
       	SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
		return SDL_MapRGB(format,0,0,0);


	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
	SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
		return SDL_MapRGB(format,0,0,0);


	actual=pixel_to_grayscale(pixels[(i+1)*surface->w+j],format);
	SDL_GetRGB(actual,format,&r,&g,&b);
	if(r==0)
		return SDL_MapRGB(format,0,0,0);
	

        return SDL_MapRGB(format,255,255,255);
}

//only use this filter when the image is only black and white (after the seuil filter) NOT FINISHED
void surface_to_lissage_filter(SDL_Surface* surface)
{
	Uint32* input_pixels=surface->pixels;
	
	SDL_Surface* copie=surface;

	SDL_PixelFormat* format=surface->format;

	SDL_LockSurface(surface);
	SDL_LockSurface(copie);

	int width=surface->w;
	int height=surface->h;

	for(int i=1;i<height-1;i++)
	{
		for(int j=1;j<width-1;j++)
		{
			input_pixels[i*width+j]=pixel_to_lissage_filter(format,copie,i,j,width,height);
		}
	}

	SDL_UnlockSurface(surface);
	SDL_UnlockSurface(copie);
}


void surface_inversion(SDL_Surface* surface)
{
	Uint32* pixels=surface->pixels;
	SDL_LockSurface(surface);
	SDL_PixelFormat* format=surface->format;

	Uint8 r,g,b;

	for(int i=0;i<surface->h;i++)
	{
		for(int j=0;j<surface->w;j++)
		{
			SDL_GetRGB(pixels[i*surface->w+j],format,&r,&g,&b);
			pixels[i*surface->w+j]=SDL_MapRGB(format,255-r,255-g,255-b);
		}
	}
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
	surface_to_grayscale(surface);

	//Save greyscale image
	//IMG_SavePNG(surface, "test_greyscale.png");

	//With medianfilter
        surface_to_medianfilter(surface);

        //Save the surface with median filter
        //IMG_SavePNG(surface,"test_median.png");
	
	//With mediumFilter
	surface_to_mediumfilter(surface);

	//Save the surface with medium filter
	IMG_SavePNG(surface,"test_medium.png");
	
	//With seuil adaptatif filter
	surface_to_seuilfilter(surface);

	//Save the surface with seuil adaptatif filter
	IMG_SavePNG(surface,"test_seuil.png");

	//With B&W filter
        //surface_to_blackandwhite(surface);

        //Save B & W image
        //IMG_SavePNG(surface,"test_B_&_W.png");
	

	//With lissage filter
	surface_to_lissage_filter(surface);

	//Save with lissage filter
	IMG_SavePNG(surface,"test_lissage.png");

	//Inversion
	//surface_inversion(surface);

	//Save Inversion
	//IMG_SavePNG(surface,"test_inversion.png");


	return EXIT_SUCCESS;
}
