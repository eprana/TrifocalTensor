#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <Eigen/Geometry>
#include <Eigen/SVD>
#include <ios>

#include "MathIO.hpp"
#include "draw.hpp"
#include "Tensor.hpp"
#include "functions.hpp"

static const size_t BYTES_PER_PIXEL = 32;


int main(int argc, char *argv[])
{
  // Init SDL image
  if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
    std::cerr << "error IMG_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Creation of variables
  // Tensor
  Tensor tensor(3, 3, 3);
  // Vectors
  VectorXd x = VectorXd::Zero(2);
  VectorXd b = VectorXd::Zero(4);
  VectorXd t = VectorXd::Zero(27);
  // Matrices
  MatrixXd A = MatrixXd::Zero(28,27);
  MatrixXd B = MatrixXd::Zero(4,2);
  // Images
  SDL_Surface* images[3];

  // Lists
  Eigen::MatrixXd list1;
  Eigen::MatrixXd list2;
  Eigen::MatrixXd list3;
  //Eigen::MatrixXd* lastList = NULL;

  // Saving repository
  std::string repository = "/tmp/";

  if(readArguments(argc, argv, images, list1, list2, list3, repository)) {
    return EXIT_FAILURE;
  }
  
  // Init screen surface
  if(SDL_Init(SDL_INIT_VIDEO) == -1){
    std::cerr << "error SDL_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Create a screen surface that will include the 3 images
  SDL_Surface *screen = SDL_SetVideoMode(images[0]->w + images[1]->w + images[2]->w, images[0]->h, BYTES_PER_PIXEL, SDL_HWSURFACE);
  SDL_WM_SetCaption("Trifocal Tensor", NULL);

  // Blit the images on the surface  
  SDL_Rect imageOffset;
  imageOffset.x = 0;
  imageOffset.y = 0;
  SDL_BlitSurface(images[0], NULL, screen, &imageOffset);
  imageOffset.x = images[0]->w;
  SDL_BlitSurface(images[1], NULL, screen, &imageOffset);
  imageOffset.x = images[0]->w + images[1]->w;
  SDL_BlitSurface(images[2], NULL, screen, &imageOffset);


  // Some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;

  bool done = false;
  while(!done) {

     calculateTensor(list1, list2, list3, t, tensor, A);

    // Draw points on images[0]
    for(int i=0; i<list1.rows(); ++i)
        fill_circle(screen, list1(i,0), list1(i,1), 3, red);

    // Draw points on images[1]
    for(int i=0; i<list2.rows(); ++i)
       fill_circle(screen, list2(i,0)+images[0]->w, list2(i,1), 3, blue);

    // Draw points on images[2]
    for(int i=0; i<list3.rows(); ++i)
        fill_circle(screen, list3(i,0)+images[0]->w+images[1]->w, list3(i,1), 3, yellow);

    // Display everything
    SDL_Flip(screen);

                    
    SDL_Event e;
    while(SDL_PollEvent(&e)) {

      // Mouse event
      if(e.type == SDL_MOUSEBUTTONDOWN) {
        // Left clic
        if(e.button.button == SDL_BUTTON_LEFT) {
          //if(e.button.x <= images[0]->w) {
            updateMatrix( list1, (float)e.button.x, (float)e.button.y, 1.0, repository + "/myList1.mat");
            //lastList = &list1;
          //}

          //if(images[0]->w < e.button.x && e.button.x <= images[1]->w + images[0]->w) {
            updateMatrix( list2, (float)e.button.x - images[0]->w, (float)e.button.y, 1.0, repository + "/myList2.mat");
            //lastList = &list2;
          //}

          //if(images[0]->w + images[1]->w < e.button.x && e.button.x <= images[2]->w + images[1]->w + images[0]->w) {
            updateMatrix( list3, (float)e.button.x - images[0]->w - images[1]->w, (float)e.button.y, 1.0, repository + "/myList3.mat");
            //lastList = &list3;
          //}

          //if( (list2.rows() > 7 && list3.rows() >7) && (list2.rows()==list3.rows() && list1.rows() == list2.rows()-1 )) {
            firstTransfert(list1, list2, list3, tensor, repository);
            //lastList = &list1;
          //}
          
           //if( (list1.rows() > 7 && list3.rows() >7) && (list1.rows()==list3.rows() && list2.rows() == list1.rows()-1 )) {
            secondTransfert(list1, list2, list3, tensor, repository);
            //lastList = &list2;
           //}

           //if( (list1.rows() > 7 && list2.rows() > 7 ) && (list1.rows() == list2.rows() && list3.rows() == list2.rows()-1 )) {
            thirdTransfert(list1, list2, list3, tensor, repository);
            //lastList = &list3;
           //}
          
          
          
   
        }
      }

      // Keyboard events
      /*if(e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym){
                   
                    case SDLK_u:
                        std::cout << "Undo" << std::endl;
                        undo(lastList);

                    break;

                    default:

                    break;

                }
      }*/


      // Closing of the window
       if(e.type == SDL_QUIT) {
        done = true;
        break;
      }
    }
  }
 
          

  // Quit sdl
  SDL_FreeSurface(images[0]); 
  SDL_FreeSurface(images[1]); 
  SDL_FreeSurface(images[2]); 
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}



