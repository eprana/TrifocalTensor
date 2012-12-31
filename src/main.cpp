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

static const size_t BYTES_PER_PIXEL = 32;

void updateMatrix(MatrixXd &list, float newX, float newY, float newZ, const std::string &filename) {
  MatrixXd tmp = list;
  list.resize(list.rows() + 1, list.cols());
  list << tmp, newX, newY, newZ;
  kn::saveMatrix(list, filename);
}

int main(int argc, char *argv[])
{
  // Init SDL image
  if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
    std::cerr << "error IMG_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Creation of variables
  Tensor tensor(3, 3, 3);
  VectorXd x = VectorXd::Zero(2);
  VectorXd b = VectorXd::Zero(4);
  VectorXd t = VectorXd::Zero(27);
  MatrixXd A = MatrixXd::Zero(28,27);
  MatrixXd B = MatrixXd::Zero(4,2);
  SDL_Surface *image1;
  SDL_Surface *image2;
  SDL_Surface *image3;

  // Load some images
  
  if(argc <= 1){
	  image1 = IMG_Load("input/image1.jpg");
	  image2 = IMG_Load("input/image2.jpg");
	  image3 = IMG_Load("input/image3.jpg");
	  if(image1 == 0 || image2 == 0 || image3 == 0){
		std::cerr << "error loading images" << std::endl;
		return 0;
	  }
	}
	if(argc >=4){
		image1 = IMG_Load(argv[1]);
		image2 = IMG_Load(argv[2]);
		image3 = IMG_Load(argv[3]);
		if(image1 == 0 || image2 == 0 || image3 == 0){
		std::cerr << "error loading images" << std::endl;
		return 0;
		}
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
 // if (argc == 1){
  	  kn::loadMatrix(list1,"input/list1.list");
	  std::ofstream list1File;
	  kn::loadMatrix(list2,"input/list2.list");
	  std::ofstream list2File;
	  kn::loadMatrix(list3,"input/list3.list");
	  std::ofstream list3File;
  /*}
  if(argc == 4){
	  kn::loadMatrix(list1,"input/emptyList.list");
	  std::ofstream list1File;
	  kn::loadMatrix(list2,"input/emptyList.list");
	  std::ofstream list2File;
	  kn::loadMatrix(list3,"input/emptyList.list");
	  std::ofstream list3File;
   }*/
   /*if(argc >= 5){
   	  kn::loadMatrix(list1,argv[4]);
	  std::ofstream list1File;
	  kn::loadMatrix(list2,argv[5]);
	  std::ofstream list2File;
	  kn::loadMatrix(list3,argv[6]);
	  std::ofstream list3File;
   }*/
  


  // Save a list
  kn::saveMatrix(list1,"/tmp/myList.mat");

  // Some colors
  Uint32 red  = 0xffff0000;
  Uint32 blue = 0xff0000ff;
  Uint32 yellow = 0xffffff00;

  bool done = false;
  while(!done) {

    // Calculation of the matrix A in At = 0
    if(list1.rows() >= 7 && list2.rows() >= 7) {
      for(int p=0; p<7; ++p) {
        for(int i=0; i<2; ++i) {
          for(int l=0; l<2; ++l) {
            for(int k=0; k<3; ++k) {
              A(4*p + 2*i + l, 9*k + 3*i + l) -= list1(p,k)*list2(p,2)*list3(p,2);
              A(4*p + 2*i + l, 9*k + 3*i + 2) += list1(p,k)*list2(p,2)*list3(p,l);
              A(4*p + 2*i + l, 9*k + 6 + l) += list1(p,k)*list2(p,i)*list3(p,2);
              A(4*p + 2*i + l, 9*k + 8) -= list1(p,k)*list2(p,i)*list3(p,l);
            }

          }
        }
      } 
    }

    // Apply the SVD
    Eigen::JacobiSVD<MatrixXd> jacobiA;
    jacobiA.compute(A, ComputeThinU | ComputeThinV);
    MatrixXd U = jacobiA.matrixU();
    MatrixXd V = jacobiA.matrixV();


    // Calculate t
    for(int i=0; i< V.rows(); ++i) {
      t(i) = V(i, V.cols() -1);     
    }

    // Put t in T
    for(int i = 0; i< tensor.getI(); ++i) {
      for(int j = 0; j<tensor.getJ(); ++j) {
        for(int k=0; k<tensor.getK(); ++k) {
          tensor.setT(i,j,k, t(9*k + 3*i + j));
        }
      }
    } 

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
            updateMatrix( list1, (float)e.button.x, (float)e.button.y, 1.0, "/tmp/myList1.mat");
          }

          if(image1->w < e.button.x && e.button.x <= image2->w + image1->w) {
            updateMatrix( list2, (float)e.button.x - image1->w, (float)e.button.y, 1.0, "/tmp/myList2.mat");
          }

          if(image1->w + image2->w < e.button.x && e.button.x <= image3->w + image2->w + image1->w) {
            updateMatrix( list3, (float)e.button.x - image1->w - image2->w, (float)e.button.y, 1.0, "/tmp/myList3.mat");
          }

          // Calculation of the matrix B in Bx = 0 for the transfert on the first image
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


          // Calculation of the matrix B in Bx = 0 for the transfert on the second image
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

          // Apply the SVD
          Eigen::JacobiSVD<MatrixXd> jacobiB;
          jacobiB.compute(B, ComputeThinU | ComputeThinV);
          x = jacobiB.solve(b);

          // Add the point to list2
          updateMatrix( list2, x(0), x(1), 1.0, "/tmp/myList2.mat");

          }

 

          // Calculation of the matrix B in Bx = b for the transfert on the third image
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
  SDL_FreeSurface(image1); 
  SDL_FreeSurface(image2); 
  SDL_FreeSurface(image3); 
  IMG_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}



