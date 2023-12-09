#include <SDL2/SDL.h>
#include "../rotate/include/rotate.h"
#include "stdlib.h"
#include "stdio.h"
#include "err.h"
#include "SDL2/SDL_image.h"
#define TRESH  0.5;




void applySobelConvolution(int height, int width, Uint32* pixels,SDL_PixelFormat* format, 
                           int sobelX[3][3], int sobelY[3][3], 
                           Uint32 magnitudes[width*height], Uint32 angles[width*height])
{   
    
    for (int y = 1; y < height - 1; y++) 
    {
        for (int x = 1; x < width - 1; x++) 
        {
            int gx = 0;
            int gy = 0;
            for (int j = -1; j <= 1; j++) 
            {
                for (int i = -1; i <= 1; i++) 
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(pixels[(y + j) * width + (x + i)], format, &r, &g, &b);

                    gx += r * sobelX[i + 1][j + 1];
                    gy += r * sobelY[i + 1][j + 1]; 
                }
            }

            int magnitude = sqrt(gx * gx + gy * gy);

            if(magnitude > 255)
            {
                magnitude = 255;
            }

            magnitudes[y * width + x] = SDL_MapRGB(format, magnitude, magnitude, magnitude);

            if(gy != 0)
            {
                angles[y * width + x] = atan(gx / gy);
            }
            else
            {
                angles[y * width + x] = 0.0;
            }
            

        }
    }   
    
    

}


void nonMaximaSuppression(int height, int width, Uint32* pixels, 
                          Uint32 magnitudes[width*height], Uint32 angles[width*height])
{
    for (int y = 1; y < height - 1; y++) 
    {
        for (int x = 1; x < width - 1; x++) 
        {
            int current = y * width + x;
            float angle = angles[current] * (180.0 / M_PI);
            if (angle < 0)
            {
                angle += 180;
            }
            float magnitude = magnitudes[current];
            
            int x1, y1, x2, y2;
            
            if (angle >= 157.5 || angle < 22.5) 
            {
                x1 = x + 1;
                y1 = y;
                x2 = x - 1;
                y2 = y;
            } 
            else if (angle >= 22.5 && angle < 67.5) 
            {
                x1 = x + 1;
                y1 = y - 1;
                x2 = x - 1;
                y2 = y + 1;
            }
            else if (angle >= 67.5 && angle < 112.5) 
            {
                x1 = x;
                y1 = y - 1;
                x2 = x;
                y2 = y + 1;
            }
            else 
            {
                x1 = x + 1;
                y1 = y + 1;
                x2 = x - 1;
                y2 = y - 1;
            }

            
            if (magnitude < magnitudes[y1 * width + x1] || magnitude < magnitudes[y2 * width + x2]) 
            {
                pixels[current] = 0; 
            } 
            else 
            {
                pixels[current] = magnitudes[current];  
            }
        }
    }
}


void applyThresholding(int height, int width, Uint32* pixels, 
                          Uint32 magnitudes[width*height], 
                          SDL_PixelFormat* format)
{
    int low_threshold = 0.3 * 255;
    int high_threshold = 0.7 * 255;

    int length = width * height;

    for (int i = 0; i < length; i ++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(magnitudes[i], format, &r, &g, &b);

        if(r < low_threshold)
        {

            pixels[i] = 0;
            magnitudes[i] = 0;

        }
        else if(r > high_threshold)
        {
            magnitudes[i] = 1;
        }
    }
}


void hysteresisAnalysis(int height, int width, Uint32* pixels, 
                          Uint32 magnitudes[width*height])
{
    for (int y = 1; y < height - 1; y++) 
    {
        for (int x = 1; x < width - 1; x++) 
        {
            if (magnitudes[y*width + x] != 0 && magnitudes[y*width + x] != 1)
            {
                int found = 0;
                for (int j = -1; j <= 1; j++) 
                {
                    for (int i = -1; i <= 1; i++) 
                    {
                        if(magnitudes[(y + j) * width + (x + i)] == 1)
                        {
                            found = 1;
                            break;
                        }
                    }
                    if(found == 1)
                    {
                        break;
                    }
                }
                if(found == 0)
                {
                    magnitudes[y*width + x] = 0;
                    pixels[y*width + x] = 0;
                }
            }
        }
    }
}




void houghTransform(int height, int width, Uint32* pixels,SDL_PixelFormat* format)

{

    int max_rho = (int)sqrt((width * width) + (height * height));

    int accumulator[max_rho][180];



    
    for(int i = 0; i< max_rho; i++) // accumulator initialization
    {
        for (int j = 0; j< 180; j++)
        {
            accumulator[i][j] = 0;
        }
    }



    for (int y = 1; y < height-1; y++) // Populates the accumulator with rho and theta values
    {
        for (int x = 1; x < width-1; x++) 
        {
            if (pixels[y * width + x] != 0) 
            {
                for (int theta = 0; theta < 180; theta++) 
                {
                    int corresponding_rho = (int)(x * cos(theta * M_PI/180) + y * sin(theta * M_PI/180));
                    accumulator[corresponding_rho][theta]++;
                }
            }
        }
    }



    double max = 0;

    for(int i = 0; i< max_rho; i++) // Finds the max value of the accumulator to prune the number of lines 
    {
        for(int j = 0; j< 180; j++)
        {
            if (accumulator[i][j] > max)
            {
                max = accumulator[i][j];
            }
        }
    }

    double hough_treshold =   max * TRESH; // Can be adjusted but works fine to draw coherent lines, only the 10 best of them will be kept

    struct Line
    {
        int rho;
        int _theta;
    };


    struct Line verticalLines[10]; // Stores the Vlines
    struct Line horizontalLines[10]; // Stores the Hlines


    for(int i = 0; i<10; i++)           // arrays initialisation, (190 > 180, the angle cannot be found in the accumulator thus the cell is empty, 0 could be a value for an existing angle)
    {
        struct Line null_line;
        null_line._theta = 190;
        null_line.rho = 190;

        verticalLines[i] = null_line;
        horizontalLines[i] = null_line;
    }



    double vertical_lineTreshold = 40; // Minimal gap between the Vlines
    double horizontal_lineTreshold = 70; // Minimal gap between the Hlines



    // DETECTS THE 10 VERTICAL LINES FOR THE GRID

    for (int rhoIndex = 0; rhoIndex < max_rho; rhoIndex++)
    {
        for (int theta = 0; theta < 180; theta++) 
        {   
            if ((theta > 175 || theta < 5) && accumulator[rhoIndex][theta] > hough_treshold) 
            {
                struct Line currentLine;
                currentLine.rho = rhoIndex;
                currentLine._theta = theta;

                double x1  = currentLine.rho * cos(currentLine._theta * M_PI / 180);

                for(int i = 0; i< 10; i++)
                {
                    struct Line ithLine = verticalLines[i];

                    double x2  = ithLine.rho * cos(ithLine._theta * M_PI / 180);

                    if (ithLine.rho == 190 && ithLine._theta == 190)
                    {
                        verticalLines[i] = currentLine;
                        break;
                    }
                    if (fabs(x1- x2) < vertical_lineTreshold)
                    {
                        verticalLines[i].rho = (verticalLines[i].rho + currentLine.rho)/2;
                        verticalLines[i]._theta = (verticalLines[i]._theta + currentLine._theta)/2;
                        break;
                    }

                }
            }
        }
    }


    // DRAWS THE 10 VERTICAL LINES OF THE GRID

    for(int i = 0; i< 10; i++)
    {
        int theta = verticalLines[i]._theta;
        int rho = verticalLines[i].rho;

        double cosTheta = cos(theta * M_PI / 180);
        double sinTheta = sin(theta * M_PI / 180);

        int d = 10000;

        double x0 = rho * cosTheta;
        double y0 = rho * sinTheta;

        int x1 = (int)(x0 + d * (-sinTheta));
        int y1 = (int)(y0 + d * (cosTheta));

        for (int i = 0; i <= d; i++) 
        {
            int x = x1 + i * (x0 - x1) / d;
            int y = y1 + i * (y0 - y1) / d;

            if (x >= 0 && x < width && y >= 0 && y < height) 
                {
                    pixels[y*width + x] = SDL_MapRGB(format, 0, 255, 0);
                }   
        }   
    }



    int test = 150;
    // DETECTS THE 10 HORIZONTAL LINES FOR THE GRID

    for (int rhoIndex = 0; rhoIndex < max_rho; rhoIndex++)
    {
        for (int theta = 85; theta < 95; theta++) 
        {
            if (accumulator[rhoIndex][theta] > hough_treshold) 
            {
                struct Line currentLine;
                currentLine.rho = rhoIndex;
                currentLine._theta = theta;


                double y1  = currentLine.rho * sin(currentLine._theta * M_PI / 180);



                for(int i = 0; i< 10; i++)
                {
                    struct Line ithLine = horizontalLines[i];

                    double y2  = ithLine.rho * sin(ithLine._theta * M_PI / 180);

                    if(fabs(y1- y2) > test)
                    {
                        double cosTheta = cos(theta * M_PI / 180);
                        double sinTheta = sin(theta * M_PI / 180);

                        int d = 10000;

                        double x0 = rhoIndex * cosTheta;
                        double y0 = rhoIndex * sinTheta;

                        int x1 = (int)(x0 - d * (-sinTheta));
                        y1 = (int)(y0 - d * (cosTheta));

                        for (int i = 0; i <= d; i++) 
                        {
                            int x = x0 + i * (x1 - x0) / d;
                            int y = y0 + i * (y1 - y0) / d;

                            if (x >= 0 && x < width && y >= 0 && y < height) 
                            {
                                pixels[y*width + x] = SDL_MapRGB(format, 0, 255, 0);
                            }
                        }
                    }

                    if (ithLine.rho == 190 && ithLine._theta == 190)
                    {

                        horizontalLines[i] = currentLine;
                        break;
                    }
                    if (fabs(y1- y2) < horizontal_lineTreshold)
                    {
                        horizontalLines[i].rho = (horizontalLines[i].rho + currentLine.rho)/2;
                        horizontalLines[i]._theta = (horizontalLines[i]._theta + currentLine._theta)/2;
                        break;
                    }

                }






                

                





            }
        }
    }


    // DRAWS THE 10 HORIZONTAL LINES OF THE GRID

    for(int i = 0; i< 10; i++)
    {
        int theta = horizontalLines[i]._theta;
        int rho = horizontalLines[i].rho;

        double cosTheta = cos(theta * M_PI / 180);
        double sinTheta = sin(theta * M_PI / 180);

        int d = 10000;

        double x0 = rho * cosTheta;
        double y0 = rho * sinTheta;

        int x1 = (int)(x0 - d * (-sinTheta));
        int y1 = (int)(y0 - d * (cosTheta));

        for (int i = 0; i <= d; i++) 
        {
            int x = x0 + i * (x1 - x0) / d;
            int y = y0 + i * (y1 - y0) / d;

            if (x >= 0 && x < width && y >= 0 && y < height) 
                {
                    pixels[y*width + x] = SDL_MapRGB(format, 0, 255, 0);
                }
        }   
    }
    

}


void lineremoval(SDL_Surface* image, int width, int height,
                 SDL_PixelFormat* format, Uint32* pixels,
                 int i, int j)
{
    if (i < 0 || i >= width || j < 0 || j >= height)
    {
        return;
    }

    if (pixels[j * width + i] != 0) 
    {
        pixels[j * width + i] = SDL_MapRGB(format, 0, 0, 0);
        lineremoval(image, width, height, format, pixels, i - 1, j - 1);
        lineremoval(image, width, height, format, pixels, i, j - 1);
        lineremoval(image, width, height, format, pixels, i + 1, j - 1);
        lineremoval(image, width, height, format, pixels, i - 1, j);
        lineremoval(image, width, height, format, pixels, i + 1, j);
        lineremoval(image, width, height, format, pixels, i - 1, j + 1);
        lineremoval(image, width, height, format, pixels, i, j + 1);
        lineremoval(image, width, height, format, pixels, i + 1, j + 1);
    }
}


void linedetection(SDL_Surface* image, int width, int height,
                               SDL_PixelFormat* format, Uint32* pixels)
{
    lineremoval(image,width,height,format,pixels, 0, 0);
    for(int i = 0; i < width; i++)
    {
        if (i == 0 || i == width-1)
        {
            for (int j = 0; j< height; j++)
            {
                if(pixels[j*width + i] != 0)
                {
                    lineremoval(image,width,height,format,pixels, i, j);
                }
            }
        }
        if(pixels[i] != 0)
        {
            lineremoval(image,width,height,format,pixels,i,0);
        }
        if(pixels[(height-1) *width + i] != 0)
        {
            lineremoval(image,width,height,format,pixels,i,pixels[(height-1) *width + i]);
        }
    }

}




void findAndExtractGridSquares(SDL_Surface* original, int width, int height,
                               SDL_PixelFormat* format, Uint32* pixels)
{
    struct coordinates 
    {
        double x;
        double y;
    };


    struct coordinates intersection[100]; // 4 vertices of the grid

    for(int i  = 0; i< 10; i++)
    {
        struct coordinates current;
        current.x = 0;
        current.y = 0;
        intersection[i] = current;
    }

/*
    
    int firstCol = 0;
    int count = 0;
    int linecount = 0;

    int gridLength = 0;

    for(int x = 0; x< width; x++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[x], format, &r, &g, &b);

        if (r == 0 && b == 0 && g == 255)
        {   

            if (count == 0)
            {

                firstCol = x;
                count ++;
            }
            else
            {
                gridLength = firstCol - x;
                break;
            }

        }

    }





    

    for(int j = 0; j < height; j++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[firstCol + j * width +10], format, &r, &g, &b);
        if (r == 0 && b == 0 && g == 255)
        {

            for (int z = -10; z < 10; z++)
            {
                int found = 0;

                Uint8 r, g, b;
                Uint8 _r, _g, _b;
                SDL_GetRGB(pixels[firstCol + (j + z +gridLength) * width +10], format, &r, &g, &b);
                SDL_GetRGB(pixels[firstCol + (j + z -gridLength) * width +10], format, &_r, &_g, &_b);




                if ((r == 0 && b == 0 && g == 255) || (_r == 0 && _b == 0 && _g == 255))
                {
                    struct coordinates current;
                    current.y= j;
                    current.x = firstCol;
                    intersection[linecount] = current;
                    linecount ++;
                    found = 1;
                    
                }

                if (found == 1)
                {
                    break;
                }
            }
            
        }

    }
    


    for (int x = 0; x< linecount; x++)
    {
        printf("X = %i", x);
        int topleftX = (int)intersection[x].x;
        int topleftY = (int)intersection[x].y;
        pixels[topleftX + topleftY * width ] = SDL_MapRGB(format, 255, 0, 0);
    }


    */


    



    int firstCol = 0;
    int secondCol = 0;
    int count = 0;
    int linecount = 0;


    for(int x = 0; x< width; x++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[x], format, &r, &g, &b);
        //pixels[x] = SDL_MapRGB(format, 255, 0, 0);

        if (r == 0 && b == 0 && g == 255)
        {   

            if (count == 0)
            {

                firstCol = x;
                count ++;
            }
            else
            {
                secondCol = x;
            }

        }

    }

    int gridLength = secondCol - firstCol;

    for (int y = 0; y < height; y++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[y], format, &r, &g, &b);
        //pixels[firstCol + y *width] = SDL_MapRGB(format, 255, 0, 0);
        //pixels[secondCol + y *width] = SDL_MapRGB(format, 255, 0, 0);
    }


    for(int j = 0; j < height; j++)
    {

        if (firstCol + j * width +10 < width * height)
        {
            Uint8 r, g, b;
            SDL_GetRGB(pixels[firstCol + j * width +10], format, &r, &g, &b);
            if (r == 0 && b == 0 && g == 255)
            {
                if (j < height/2)
                {
                    int indexToCheck = firstCol + (j + gridLength) * width +20;



                    if ((indexToCheck < width * height))
                    {

                        for(int x = 0; x <width; x++)
                        {
                    //pixels[x + (int)intersection[0].y * width] = SDL_MapRGB(format, 255, 0, 0);
        }

                        for (int z = -10; z < 10; z++)
                        {
                            int found = 0;

                            Uint8 r, g, b;

                            
                            SDL_GetRGB(pixels[firstCol + (j + z +gridLength) * width +10], format, &r, &g, &b);
                            
                            


                            if ((r == 0 && b == 0 && g == 255))
                            {
                                struct coordinates current;
                                current.y= j;
                                current.x = firstCol;
                                intersection[linecount] = current;
                                linecount ++;
                                found = 1;

                                
                            }

                            if (found == 1)
                            {
                                break;
                            }
                        }
                    }
                }
                /*else
                {
                    int indexToCheck = firstCol + (j - gridLength) * width - 20;


                    printf("%i\n", indexToCheck);
                    printf("%i\n", width * height);

                    if (indexToCheck > 0)
                    {

                        for (int z = -10; z < 10; z++)
                        {
                            int found = 0;

                            Uint8 r, g, b;

                            
                            
                            SDL_GetRGB(pixels[firstCol + (j + z - gridLength) * width +10], format, &r, &g, &b);
                            


                            if ((r == 0 && b == 0 && g == 255))
                            {
                                struct coordinates current;
                                current.y= j;
                                current.x = firstCol;
                                intersection[linecount] = current;
                                if (linecount< 100){linecount++;}
                                found = 1;

                                
                            }

                            if (found == 1)
                            {
                                break;
                            }
                        }
                    }
                    
                }
                */

                

                

                
            }
        }

    }


    printf("test");



    
    int Step = gridLength/9;
    int topleftX = (int)intersection[0].x;
    int topleftY = (int)intersection[0].y;


    int i = 1;



    for(int y = 0; y < 9; y ++)
    {
        for(int x = 0; x < 9;x ++)
        {
            
            int xcorda = topleftX + x * Step;
            int ycorda = topleftY + y * Step;

            SDL_Rect square;
            square.x = xcorda;
            square.y = ycorda;
            square.w = Step;
            square.h = Step;

            if(square.x + Step >width)
            {
                square.x = width - Step;
            }
            SDL_Surface* subImage = SDL_CreateRGBSurface(0, square.w, square.h, original->format->BitsPerPixel, original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);


            


            SDL_BlitSurface(original, &square, subImage, NULL);

            linedetection(subImage, square.w, square.h,
                               format, (Uint32*)subImage->pixels);

            char filename[100];

            sprintf(filename, "returned_images/cases/square number %i.bmp", i);

            if (SDL_SaveBMP(subImage, filename) != 0) 
            {
            SDL_Log("Failed to save the subimage: %s\n", SDL_GetError());
            }
            i++;
        }
        // i++;
    }

    SDL_Rect square;
    square.x = (int)intersection[0].x;
    square.y = (int)intersection[0].y;
 

    square.w = gridLength ;//+ Len/15;
    square.h = gridLength ;//+ Len/15;

    SDL_Surface* subImage = SDL_CreateRGBSurface(0, square.w, square.h, original->format->BitsPerPixel, original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);

    SDL_BlitSurface(original, &square, subImage, NULL);

    SDL_SaveBMP(subImage, "returned_images/cases/EXTRACTED GRID.bmp");

    
}




SDL_Surface* cannyEdgeDetection(SDL_Surface* originalImage)
{
    int sobelX[3][3] = { { -1, 0, 1 }, 
                         { -2, 0, 2 }, 
                         { -1, 0, 1 } }; // Sobel Matrix for X convolution

    int sobelY[3][3] = { { -1, -2, -1 },
                         {  0,  0,  0 }, 
                         {  1,  2,  1 } }; // Sobel Matrix for Y convolution

    int height = originalImage->h;
    int width = originalImage->w;

    Uint32* magnitudes = (Uint32*)malloc(sizeof(Uint32) * width * height);
    Uint32* angles = (Uint32*)malloc(sizeof(Uint32) * width * height);

    if (!magnitudes || !angles) {
        // Handle memory allocation failure
        fprintf(stderr, "Memory allocation failed\n");
        free(magnitudes);
        free(angles);
        return NULL;  // or handle the error accordingly
    }

    applySobelConvolution(height, width, originalImage->pixels, originalImage->format,
                          sobelX, sobelY, magnitudes, angles);



    nonMaximaSuppression(height, width, originalImage->pixels, magnitudes, angles);

    applyThresholding(height, width, originalImage->pixels, magnitudes, originalImage->format);

    hysteresisAnalysis(height, width, originalImage->pixels, magnitudes);

    free(magnitudes);
    free(angles);

    return originalImage;
}









SDL_Surface * HoughDetection(SDL_Surface* image) 
{


    SDL_Surface *original = SDL_ConvertSurface(image, image->format, 0);

    image = cannyEdgeDetection(image);


    houghTransform(image->h, image->w, image->pixels, image->format);

    
    
    findAndExtractGridSquares(original, image->w, image->h, image->format, image->pixels);




    

    return image;
}







SDL_Surface* automaticRotation(SDL_Surface* image)
{

    int width = image->w;
    int height = image->h;

    SDL_Surface *original = SDL_ConvertSurface(image, image->format, 0);




    image = cannyEdgeDetection(image);


    int max_rho = (int)sqrt((width * width) + (height * height));

    int accumulator[max_rho][180];



    
    for(int i = 0; i< max_rho; i++) // accumulator initialization
    {
        for (int j = 0; j< 180; j++)
        {
            accumulator[i][j] = 0;
        }
    }

    Uint32* pixels = image->pixels;



    for (int y = 1; y < height-1; y++) // Populates the accumulator with rho and theta values
    {
        for (int x = 1; x < width-1; x++) 
        {
            if (pixels[y * width + x] != 0) 
            {
                for (int theta = 0; theta < 180; theta++) 
                {
                    int corresponding_rho = (int)(x * cos(theta * M_PI/180) + y * sin(theta * M_PI/180));
                    accumulator[corresponding_rho][theta]++;
                }
            }
        }
    }



    int max = 0;

    for(int i = 0; i< max_rho; i++) // Finds the max value of the accumulator to prune the number of lines 
    {
        for(int j = 0; j< 180; j++)
        {
            if (accumulator[i][j] > max)
            {
                max = accumulator[i][j];
            }
        }
    }


    int hough_treshold = 0.7 * max; // Can be adjusted but works fine to draw coherent lines, only the 10 best of them will be kept

    int histo[180];
    for (int j = 0; j< 180; j++)
    {
        histo[j] = 0;
    }




    for (int rhoIndex = 0; rhoIndex < max_rho; rhoIndex++)
    {
        for (int theta = 45; theta < 135; theta++) 
        {
            if (accumulator[rhoIndex][theta] > hough_treshold) 
            {

                histo[theta]+=1;

            }

        }

    }



    int maxi = 0;
    int angleT = 0;
    for(int j = 0; j <180; j++)
    {
        if(histo[j] > maxi)
        {
            maxi = histo[j];
            angleT = j;
        }
    }


    int deg = 360 - (angleT) + 90;


    original = rotation(original,deg);



    SDL_Rect square;
    square.x = (original->w - width)/2;
    square.y = (original->h - height)/2;


    square.w = width;
    square.h = height;

    SDL_Surface* subImage = SDL_CreateRGBSurface(0, square.w, square.h, original->format->BitsPerPixel, original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);

    SDL_BlitSurface(original, &square, subImage, NULL);




    return subImage;
    


}


