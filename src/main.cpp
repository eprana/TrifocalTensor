#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <Eigen/Geometry>
#include <Eigen/SVD>

static const size_t WINDOW_WIDTH = 1000;
static const size_t WINDOW_HEIGHT = 800;
static const size_t BYTES_PER_PIXEL = 32;



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
    
    

    // Creation of the variables
    SDL_Surface *screen = NULL;
    screen = SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BYTES_PER_PIXEL, SDL_HWSURFACE);
    if (screen == NULL) {
        std::cerr << " Error in the loading of the video mode" << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
    }
    SDL_WM_SetCaption("The Best Trifocal Tensor Ever", NULL);


    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 201, 191, 126));

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