#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer,texture,NULL, NULL);
    SDL_RenderPresent(renderer); 
}

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
}


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface = IMG_Load(path);
    if (!surface)
    {
        errx(EXIT_FAILURE, "INIT SURFACE ERROR: %s", IMG_GetError());
    }
    SDL_Surface* new_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface);
    if (!new_surface)
    {
        errx(EXIT_FAILURE, "RGB SURFACE ERROR: %s", SDL_GetError());
    }
    return new_surface;

}



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



    double vertical_lineTreshold = 30; // Minimal gap between the Vlines
    double horizontal_lineTreshold = 30; // Minimal gap between the Hlines



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


    struct coordinates intersection[4]; // 4 vertices of the grid

    for(int i  = 0; i< 4; i++)
    {
        struct coordinates current;
        current.x = 0;
        current.y = 0;
        intersection[i] = current;
    }


    int xcount = 0;
    int ycount = 0;

    for(int x = 0; x< width; x++)
    {
        Uint8 r, g, b;
        SDL_GetRGB(pixels[x], format, &r, &g, &b);

        if (r == 0 && b == 0 && g == 255)
        {   
            xcount ++;

            if (xcount == 1)  
            {
                for(int y = 0; y<height;y++)
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(pixels[y* width + x +1], format, &r, &g, &b);
                    if (r == 0 && b == 0 && g == 255)
                    {
                        struct coordinates current;
                        current.x = x;
                        current.y = y;

                        ycount++;

                        pixels[y * width  + x] = SDL_MapRGB(format, 255, 0, 0);


                        if (ycount == 1)
                        {
                            intersection[0] = current; // 1st Intersection (TOP LEFT)
                        }
                        else if (ycount == 10)
                        {
                            intersection[1] = current; // 2nd Intersection (Bottom LEFT)
                            break;

                        }
                    }

                }
            }

            else if (xcount == 10)
            {
                ycount = 0;

                for(int y = 0; y<height;y++)
                {

                    Uint8 r, g, b;
                    SDL_GetRGB(pixels[y* width + x-1], format, &r, &g, &b);

                    if (r == 0 && b == 0 && g == 255)
                    {
                        struct coordinates current;
                        current.x = x;
                        current.y = y;

                        ycount++;


                        pixels[y * width  + x] = SDL_MapRGB(format, 255, 0, 0);


                        if (ycount == 1)
                        {
                            intersection[2] = current; // 3nd Intersection (Top Right)
                        }
                        else if (ycount == 10)
                        {
                        
                            intersection[3] = current; // 4th Intersection (Bottom Right)

                            break;
                        }
                    }

                }
                break;
            }
            else
            {
                ycount = 0;

                for(int y = 0; y<height;y++)
                {
                    Uint8 r, g, b;
                    SDL_GetRGB(pixels[y* width + x + 1], format, &r, &g, &b);

                    if (r == 0 && b == 0 && g == 255)
                    {
                        pixels[y * width  + x] = SDL_MapRGB(format, 255, 0, 0);

                        ycount++;


                        if (ycount == 10)
                        {
                            break;
                        }
                    }
                }
            }

        }

    }


    int xLen = abs((int)(intersection[2].x - intersection[0].x));
    int xStep = xLen/9;
    int yLen = abs((int)(intersection[1].y - intersection[0].y));
    int yStep = yLen/9;
    int topleftX = (int)intersection[0].x;
    int topleftY = (int)intersection[0].y;

    int i = 1;


    for(int y = 0; y <  yLen - yStep; y += yStep)
    {
        for(int x = 0; x <  xLen - xStep; x += xStep)
        {
            
            int xcorda = topleftX + x;
            int ycorda = topleftY + y;

            SDL_Rect square;
            square.x = xcorda = topleftX + x;
            square.y = ycorda = topleftY + y;
            square.w = xStep;
            square.h = yStep;
            SDL_Surface* subImage = SDL_CreateRGBSurface(0, square.w, square.h, original->format->BitsPerPixel, original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);

            


            SDL_BlitSurface(original, &square, subImage, NULL);

            linedetection(subImage, square.w, square.h,
                               format, (Uint32*)subImage->pixels);

            char filename[100];

            sprintf(filename, "PICTURES/square number %i.bmp", i);

            if (SDL_SaveBMP(subImage, filename) != 0) 
            {
            SDL_Log("Failed to save the subimage: %s\n", SDL_GetError());
            }
            i++;

        }
        i++;

    }


    SDL_Rect square;
    square.x = (int)intersection[0].x;
    square.y = (int)intersection[0].y;
    square.w = abs((int)(intersection[2].x - intersection[0].x));
    square.h = abs((int)(intersection[1].y - intersection[0].y));

    SDL_Surface* subImage = SDL_CreateRGBSurface(0, square.w, square.h, original->format->BitsPerPixel, original->format->Rmask, original->format->Gmask, original->format->Bmask, original->format->Amask);

    SDL_BlitSurface(original, &square, subImage, NULL);


    SDL_SaveBMP(subImage, "PICTURES/EXTRACTED GRID.bmp");
}







void edgeDetection(SDL_Surface* image, SDL_Surface* original) 
{
    
    int sobelX[3][3] = { { -1, 0, 1 }, 
                         { -2, 0, 2 }, 
                         { -1, 0, 1 } }; // Sobel Matrix for X convolution

    int sobelY[3][3] = { { -1, -2, -1 },
                         {  0,  0,  0 }, 
                         {  1,  2,  1 } }; // Sobel Matrix for Y convolution


    int height = image->h; 
    int width = image->w;

    SDL_PixelFormat* format = image->format; // Common format used everywhere

   
    Uint32* pixels = (Uint32*)image->pixels; // Pixels of the surface

    Uint32 magnitudes[width*height]; // Gradient obtained with Sobel

    Uint32 angles[width*height]; // Angles used for non maxima removal

    applySobelConvolution(height, width, pixels,format,sobelX, sobelY, magnitudes, angles);

    nonMaximaSuppression( height, width, pixels, magnitudes, angles);

    applyThresholding( height, width, pixels, magnitudes, format);

    hysteresisAnalysis(height, width, pixels, magnitudes);

    houghTransform(height, width, pixels, format);

    findAndExtractGridSquares(original, width, height, format, pixels);

    SDL_UnlockSurface(image);
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
    SDL_Surface* original_surface = load_image(argv[1]);

    SDL_SetWindowSize(window,surface->w,surface->h);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    edgeDetection(surface,original_surface);
    SDL_Texture* cannyTexture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);
    SDL_FreeSurface(original_surface);
    event_loop(renderer,texture,cannyTexture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyTexture(cannyTexture);
    SDL_Quit();


    return EXIT_SUCCESS;
}
