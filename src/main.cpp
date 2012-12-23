#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <Eigen/Geometry>
#include <Eigen/SVD>

#include "MathIO.hpp"
#include "draw.hpp"
#include "Tensor.hpp"

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


int main(int argc, char *argv[])
{
  // Init SDL image
  if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
    std::cerr << "error IMG_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Creation of variables
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;

  // Load some images
  SDL_Surface *image1 = IMG_Load("input/image1.jpg");
  SDL_Surface *image2 = IMG_Load("input/image2.jpg");
  SDL_Surface *image3 = IMG_Load("input/image3.jpg");
  if(image1 == 0 || image2 == 0 || image3 == 0){
    std::cerr << "error loading images" << std::endl;
    return 0;
  }

  // Init screen surface
  if(SDL_Init(SDL_INIT_VIDEO) == -1){
    std::cerr << "error SDL_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Create a screen surface that will include the 3 images
  SDL_Surface *screen = SDL_SetVideoMode(image1->w + image2->w + image3->w, image1->h, BYTES_PER_PIXEL, SDL_HWSURFACE);
  SDL_WM_SetCaption("Trifocal Tensor", NULL);

  // Blit the images on the surface  
  SDL_Rect imageOffset;
  imageOffset.x = 0;
  imageOffset.y = 0;
  SDL_BlitSurface(image1, NULL, screen, &imageOffset);
  imageOffset.x = image1->w;
  SDL_BlitSurface(image2, NULL, screen, &imageOffset);
  imageOffset.x = image1->w + image2->w;
  SDL_BlitSurface(image3, NULL, screen, &imageOffset);

  // Load the point lists
  Eigen::MatrixXd list1;
  Eigen::MatrixXd list2;
  Eigen::MatrixXd list3;
  kn::loadMatrix(list1,"input/list1.list");
  kn::loadMatrix(list2,"input/list2.list");
  kn::loadMatrix(list3,"input/list3.list");


  // Save a list
  kn::saveMatrix(list1,"/tmp/myList.mat");

  // Some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;

  bool done = false;
  while(!done) {


    // Draw points on image1
    for(int i=0; i<list1.rows(); ++i)
      if(list1(i,0) != 0 && list1(i,1) != 0) {
        fill_circle(screen, list1(i,0), list1(i,1), 3, red);
      }

    // Draw points on image2
    for(int i=0; i<list2.rows(); ++i)
      if(list2(i,0) != 0 && list2(i,1) != 0) {
       fill_circle(screen, list2(i,0)+image1->w, list2(i,1), 3, blue);
      }

    // Draw points on image3
    for(int i=0; i<list3.rows(); ++i)
      if(list3(i,0) != 0 && list3(i,1) != 0) {
        fill_circle(screen, list3(i,0)+image1->w+image2->w, list3(i,1), 3, yellow);
      }

    // Display everything
    SDL_Flip(screen);

    // Management of the events
    SDL_Event e;
    while(SDL_PollEvent(&e)) {

      // Mouse event
      if(e.type == SDL_MOUSEBUTTONDOWN) {
        // Left clic
        if(e.button.button == SDL_BUTTON_LEFT) {
          if(e.button.x <= image1->w) {
            kn::saveMatrix(list1,"/tmp/myList.mat");
            list1(count1,0) = e.button.x;
            list1(count1,1) = e.button.y;
            count1++;
            if(count1== 7) {
              count1 = 0;  
            } 
          }
          if(image1->w < e.button.x && e.button.x <= image2->w + image1->w) {
            std::cout << "Second image" << std::endl;
            list2(count2,0) = e.button.x - image1->w;
            list2(count2,1) = e.button.y;
            count2++;
            if(count2++ == 7) {
              count2 = 0;  
            } 
          }
          if(image1->w + image2->w < e.button.x && e.button.x <= image3->w + image2->w + image1->w) {
            std::cout << "Third image" << std::endl;
            list3(count3,0) = e.button.x - image1->w - image2->w;
            list3(count3,1) = e.button.y;
            count3++;
            if(count3++ == 7) {
              count3 = 0;  
            } 
          }
        }
      }
      // Closing of the window
       if(e.type == SDL_QUIT) {
        done = true;
        break;
      }
    }
  }
 
          

  // Quit sdl
  SDL_FreeSurface(image1); 
  SDL_FreeSurface(image2); 
  SDL_FreeSurface(image3); 
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}



