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


int main(int argc, char *argv[])
{
  // Init SDL image
  if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == -1){
    std::cerr << "error IMG_Init" << std::endl;
    return EXIT_FAILURE;
  }

  // Creation of variables
  VectorXd x(2);
  for(int i = 0; i < x.rows(); ++i){
      x(i) = 0;
  }
  VectorXd b(4);
  for(int i = 0; i < b.rows(); ++i){
      b(i) = 0;
  }
  Eigen::VectorXd t(27);
  for(int i = 0; i < t.rows(); ++i){
      t(i) = 0;
  }
  Tensor tensor(3, 3, 3);
  MatrixXd A(28, 27);
  for(int i = 0; i < A.rows(); ++i){
    for(int j= 0; j < A.cols(); ++j) {
      A(i,j) = 0;
    }
  }
  MatrixXd B(4,2);
  for(int i = 0; i < B.rows(); ++i){
    for(int j= 0; j < B.cols(); ++j) {
      B(i,j) = 0;
    }
  }

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
  std::ofstream list1File;
  kn::loadMatrix(list2,"input/list2.list");
  std::ofstream list2File;
  kn::loadMatrix(list3,"input/list3.list");
  std::ofstream list3File;
  


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
      //std::cout << t(i) << std::endl;
      
    }

    // Put t in T
    int countT = 0;
    for(int i = 0; i< tensor.getI(); ++i) {
      for(int j = 0; j<tensor.getJ(); ++j) {
        for(int k=0; k<tensor.getK(); ++k) {
          // See if there is another technique to fill T
          tensor.setT(i,j,k, t(countT));
          countT++;
        }
      }
    } 
   
/*

  // Calculation of the matrix B in Bx = 0 for p= 8 !!  and x
    if(list2.rows() == 8 && list3.rows() == 8) {
      std::cout << "Image 1" <<std::endl;
      for(int i = 0; i<2; ++i) {
        for(int j = 0; j<2; ++j) {
          for(int k = 0; k<2; ++k) {
            B(2*i + j, k) += list2(7,i)*list3(7,2)*tensor(2,j,k) - list2(7,2)*list3(7,2)*tensor(i,j,k) - list2(7,i)*list3(7,j)*tensor(2,2,k) + list2(7,2)*list3(7,j)*tensor(i,2,k);
          }
        }
      }  
      kn::saveMatrix(B, "input/b.list"); 
    

    // Apply the SVD
    Eigen::JacobiSVD<MatrixXd> jacobiB;
    jacobiB.compute(B, ComputeThinU | ComputeThinV);
    jacobiB.solve();
    for(int i=0; i < x.rows(); ++i) {
      std::cout << x(i) << std::endl;
    }
    MatrixXd Ub = jacobiB.matrixU();
    kn::saveMatrix(Ub, "input/Ub.list");
    MatrixXd Vb = jacobiB.matrixV();
    kn::saveMatrix(Vb, "input/Vb.list");


    //Calculate x
    std::cout << "Coordonnées de x" << std::endl;
    for(int i=0; i< Vb.rows(); ++i) {
      std::cout << Vb(i, Vb.cols() -1) << std::endl;
      x(0) = Vb(0, Vb.cols() -1);
      x(1) = Vb(1, Vb.cols() -1);

      //std::cout << x(i) << std::endl;
    }
    std::cout << "end x" << std::endl;

    // Write the point in the list3
    list1File.open("input/list1.list", std::ios::app);
    list1File << x(0) << " ";
    list1File << x(1) << " ";
    list1File << 1.0 << std::endl;
        
    list1File.close();
    kn::loadMatrix(list1,"input/list1.list");
  }

*/   


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

    // Calculation of the matrix B in Bx = b for p= 8 !!  and x''
    
    if(list1.rows() == 8 && list2.rows() == 8) {
      std::cout << "Calculation of B" << std::endl;
      std::cout << "Image 3" <<std::endl;
      for(int i = 0; i<2; ++i) {
        for(int j = 0; j<2; ++j) {
          for(int k = 0; k<3; ++k) {
            B(2*i + j, j) +=  list1(7,k)*(tensor(i,2,k) - list2(7,i)*tensor(2,2,k));
          }
        }
      }   

      kn::saveMatrix(B, "input/B.list"); 

    // Calculation of the vector b in Bx=b
      std::cout << "Calculation of b" << std::endl;
      for(int i = 0; i<2; ++i) {
        for(int j = 0; j<2; ++j) {
          for(int k = 0; k<3; ++k) {
            b(2*i + j) += list1(7,k)*(list2(7,i)*tensor(2,j,k) - list2(7,2)*tensor(i,j,k));
          }
        }
      } 

      kn::saveMatrix(b, "input/b.list");

   // Apply the SVD
      std::cout << "SVD on B" << std::endl;
    Eigen::JacobiSVD<MatrixXd> jacobiB;
    jacobiB.compute(B, ComputeThinU | ComputeThinV);
    x = jacobiB.solve(b);

    // Write the point in the list3
    std::cout << "Write in list3" << std::endl;
    list3File.open("input/list3.list", std::ios::app);
    list3File << x(0) << " ";
    list3File << x(1) << " ";
    list3File << 1.0 << std::endl;
        
    list3File.close();
    kn::loadMatrix(list3,"input/list3.list");
  }

          if(e.button.x <= image1->w) {
            list1File.open("input/list1.list", std::ios::app);
            list1File << (float)e.button.x << " ";
            list1File << (float)e.button.y << " ";
            list1File << (float)1.0 << std::endl;
            
            list1File.close();

            kn::loadMatrix(list1,"input/list1.list");
          }

          if(image1->w < e.button.x && e.button.x <= image2->w + image1->w) {
            list2File.open("input/list2.list", std::ios::app);
            list2File << (float)e.button.x - image1->w<< " ";
            list2File << (float)e.button.y << " ";
            list2File << (float)1.0 << std::endl;
            
            list2File.close();

            kn::loadMatrix(list2,"input/list2.list");
            /*std::cout << "Second image" << std::endl;
            list2(count2,0) = e.button.x - image1->w;
            list2(count2,1) = e.button.y;
            kn::saveMatrix(list1,"/tmp/myList2.mat");
            count2++;
            if(count2++ == 7) {
              count2 = 0;  
            } */
          }
          if(image1->w + image2->w < e.button.x && e.button.x <= image3->w + image2->w + image1->w) {
            list3File.open("input/list3.list", std::ios::app);
            list3File << (float)e.button.x - image1->w - image2->w << " ";
            list3File << (float)e.button.y << " ";
            list3File << (float)1.0 << std::endl;
            
            list3File.close();

            kn::loadMatrix(list3,"input/list3.list");
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



