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

  if(readArguments(argc, argv, images, list1, list2, list3)) {
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

  // Save a list
  kn::saveMatrix(list1,"/tmp/myList.mat");

  // Some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;

  bool done = false;
  while(!done) {

     calculateTensor(list1, list2, list3, t, tensor, A);

    // Draw points on images[0]
    for(int i=0; i<list1.rows(); ++i)
      if(list1(i,0) != 0 && list1(i,1) != 0) {
        fill_circle(screen, list1(i,0), list1(i,1), 3, red);
      }

    // Draw points on images[1]
    for(int i=0; i<list2.rows(); ++i)
      if(list2(i,0) != 0 && list2(i,1) != 0) {
       fill_circle(screen, list2(i,0)+images[0]->w, list2(i,1), 3, blue);
      }

    // Draw points on images[2]
    for(int i=0; i<list3.rows(); ++i)
      if(list3(i,0) != 0 && list3(i,1) != 0) {
        fill_circle(screen, list3(i,0)+images[0]->w+images[1]->w, list3(i,1), 3, yellow);
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
          if(e.button.x <= images[0]->w) {
            updateMatrix( list1, (float)e.button.x, (float)e.button.y, 1.0, "/tmp/myList1.mat");
          }

          if(images[0]->w < e.button.x && e.button.x <= images[1]->w + images[0]->w) {
            updateMatrix( list2, (float)e.button.x - images[0]->w, (float)e.button.y, 1.0, "/tmp/myList2.mat");
          }

          if(images[0]->w + images[1]->w < e.button.x && e.button.x <= images[2]->w + images[1]->w + images[0]->w) {
            updateMatrix( list3, (float)e.button.x - images[0]->w - images[1]->w, (float)e.button.y, 1.0, "/tmp/myList3.mat");
          }

          // Calculation of the matrix B in Bx = 0 for the transfert on the first images
          if( (list2.rows() > 7 && list3.rows() >7) && (list2.rows()==list3.rows() && list1.rows() == list2.rows()-1 )) {
            std::cout << "Transfert on the first picture" <<std::endl;
            for(int i = 0; i<2; ++i) {
              for(int j = 0; j<2; ++j) {
                for(int k = 0; k<3; ++k) {
                  B(2*i + j, 0) = list2(list2.rows()-1,i)*tensor(2,j,0) - tensor(i,j,0) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,0) + list3(list2.rows()-1,j)*tensor(i,2,0);
                  B(2*i + j, 1) = list2(list2.rows()-1,i)*tensor(2,j,1) - tensor(i,j,1) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,1) + list3(list2.rows()-1,j)*tensor(i,2,1);
                  b(2*i + j) = - (list2(list2.rows()-1,i)*tensor(2,j,2) - tensor(i,j,2) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,2) + list3(list2.rows()-1,j)*tensor(i,2,2));
                }
              }
            }           

            // Apply the SVD
            Eigen::JacobiSVD<MatrixXd> jacobiB;
            jacobiB.compute(B, ComputeThinU | ComputeThinV);
            x = jacobiB.solve(b);

            // Add the point to list1
            updateMatrix( list1, x(0), x(1), 1.0, "/tmp/myList1.mat");
          }


          // Calculation of the matrix B in Bx = 0 for the transfert on the second images
          if( (list1.rows() > 7 && list3.rows() >7) && (list1.rows()==list3.rows() && list2.rows() == list1.rows()-1 )) {
            std::cout << "Transfert on the second picture" <<std::endl;

            B = MatrixXd::Zero(4,2);
            b = VectorXd::Zero(4);

            for(int i = 0; i<2; ++i) {
              for(int j = 0; j<2; ++j) {
                for(int k = 0; k<3; ++k) {
                  B(2*i + j, i) +=  list1(list1.rows()-1,k)*  (tensor(2,j,k) - list3(list1.rows()-1,j)*tensor(2,2,k));
                  b(2*i + j) += list1(list1.rows()-1,k)*(-list3(list1.rows()-1,j)*tensor(i,2,k) + tensor(i,j,k));

                }
              }
            }  

              kn::saveMatrix(B, "input/B.list");

            // Apply the SVD
            Eigen::JacobiSVD<MatrixXd> jacobiB;
            jacobiB.compute(B, ComputeThinU | ComputeThinV);
            x = jacobiB.solve(b);

            // Add the point to list2
            std::cout << "Adding point the list2" << std::endl;
            updateMatrix( list2, x(0), x(1), 1.0, "/tmp/myList2.mat");

          }

 

          // Calculation of the matrix B in Bx = b for the transfert on the third images
          if( (list1.rows() > 7 && list2.rows() > 7 ) && (list1.rows() == list2.rows() && list3.rows() == list2.rows()-1 )) {
            std::cout << "Calculation of B and b" << std::endl;
            std::cout << "Transfert on the third picture" <<std::endl;

            B = MatrixXd::Zero(4,2);
            b = VectorXd::Zero(4);
             
            for(int i = 0; i<2; ++i) {
              for(int j = 0; j<2; ++j) {
                for(int k = 0; k<3; ++k) {
                  B(2*i + j, j) +=  list1(list1.rows()-1,k)*(tensor(i,2,k) - list2(list1.rows()-1,i)*tensor(2,2,k));
                  b(2*i + j) += list1(list1.rows()-1,k)*(-list2(list1.rows()-1,i)*tensor(2,j,k) + tensor(i,j,k));

                }
              }
            }  
            kn::saveMatrix(B, "input/B.list"); 

         // Apply the SVD
          Eigen::JacobiSVD<MatrixXd> jacobiB;
          jacobiB.compute(B, ComputeThinU | ComputeThinV);
          x = jacobiB.solve(b);

          // Add the point to list3
          updateMatrix( list3, x(0), x(1), 1.0, "/tmp/myList3.mat");
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
  SDL_FreeSurface(images[0]); 
  SDL_FreeSurface(images[1]); 
  SDL_FreeSurface(images[2]); 
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}



