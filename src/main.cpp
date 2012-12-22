#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <Eigen/Geometry>
#include <Eigen/SVD>

static const size_t WINDOW_WIDTH = 1000;
static const size_t WINDOW_HEIGHT = 800;
static const size_t BYTES_PER_PIXEL = 32;

SDL_Surface* loadPicture(SDL_Surface* pscreen, size_t width, size_t height, int r, int g, int b, int x, int y) {
    SDL_Surface* surface = NULL;
    surface = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, BYTES_PER_PIXEL, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(pscreen->format, r, g, b));
    SDL_Rect position;
    position.x = x;
    position.y = y;
    SDL_BlitSurface(surface, NULL, pscreen, &position);

    return surface;
}

int main() {
    // Load the pictures 

    // Save a list of pixels

    // Display the pictures
    // Initialisation of the SDL
     if (SDL_Init(SDL_INIT_VIDEO) == -1) {
            std::cerr << " Error in the initialisation of the SDL" << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
    }
    // Open the window
    SDL_Surface *screen = NULL;
    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BYTES_PER_PIXEL, SDL_HWSURFACE);
    if (screen == NULL) {
        std::cerr << " Error in the loading of the video mode" << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("The Best Trifocal Tensor Ever", NULL);

    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 201, 191, 126));
    

    // Creation of the variables
    SDL_Surface* firstPicture = loadPicture(screen, 220, 180, 255, 255, 255, 0, 0);
    SDL_Surface* secondPicture = loadPicture(screen, 220, 180, 255, 255, 255, 0, 200);
    SDL_Surface* thirdPicture = loadPicture(screen, 220, 180, 255, 255, 255, 0, 400);

        
    // Diplayed code
    bool done = false;
    while(!done) {


        
        // Updating of the display
        SDL_Flip(screen);

        





        // Management of the events
        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            // Closing of the window
            if(e.type == SDL_QUIT) {
                done = true;
                break;
            }
        }
    }
 
    // Free the surfaces
    SDL_FreeSurface(firstPicture);
    SDL_FreeSurface(secondPicture);
    SDL_FreeSurface(thirdPicture);

    SDL_Quit(); 

    // Select the points on two pictures
        int clickedPoints = 7;

        // For each clic : ++clikedPoints

    // Fill the matrix A in At = 0

    Eigen::MatrixXf A(4*clickedPoints, 27);
    // For the different points
    for(int p = 0; p < clickedPoints; ++p) {
        // In all the lines
        for(int i = 0; i < 2; ++i) {
            // In all the columns
            for( int l = 0; l < 2; ++l) {
                // Offset in the columns
                for(int k = 0; k < 3; ++k) {
                    A(4*p + 2*i + l, 9*k + 3*i + l) = 0; 
                    A(4*p + 2*i + l, 9*k + 3*i + 2) = 0;
                    A(4*p + 2*i + l, 9*k + 6 + l) = 0 ;
                    A(4*p + 2*i + l,9*k + 8) = 0 ;
                }

            }
        }
    }

    // Transform the tensor matrix in a tensor vector

    // Calculate the tensor T with the SVD

    // Fill the matrix B in Bx = 0

    // Calculate the third point with SVD

    // Display the point on the third picture




    return EXIT_SUCCESS;
}