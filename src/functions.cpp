#include "functions.hpp"

void updateMatrix(MatrixXd &list, float newX, float newY, float newZ, const std::string &filename) {
    if(list.rows() == 0) {
        list.resize(1,3);
        list(0,0) = newX;
        list(0,1) = newY;
        list(0,2) = newZ;

        kn::saveMatrix(list, filename);
    }
    else {
        MatrixXd tmp = list;
        list.resize(list.rows() + 1, list.cols());
        list << tmp, newX, newY, newZ;
        kn::saveMatrix(list, filename);
    }
}

bool readArguments(int argc, char** argv, SDL_Surface** images, Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3, std::string& repository){

  // English help
  if( (argc == 2) && (strcmp("-h",argv[1]) == 0)) {
    std::cout << "TRIFOCAL TENSOR" << std::endl;

    std::cout << " This logiciel enable to find a point on a given image of a 3D scene by clicking the same point on two other images of the same scene on a different angle."<< std::endl << std::endl;
    std::cout << " In order to do that, you have to click, in the same order, on 7 similar points on the three images" << std::endl << std::endl;
    std::cout << " Then, if you pick the point on two images, it will calculate the third one on the last image" << std::endl << std::endl << std::endl;
    std::cout << " By default, the lists of clicked points are saved in your tmp repository. "<< std::endl << std::endl;

    std::cout << "./bin/trifocal  [OPTIONS] \t launch the program with default pictures and list of points" << std::endl << std::endl;
    std::cout << "[image1].jpg [image2].jpg [image3].jpg : \t launch the program with the three given images" << std::endl << std::endl;
    std::cout << "[image1].jpg [image2].jpg [image3].jpg [repository]: \t launch the program with the three given images and the path to the repository where the list of cliked points will be saved" << std::endl << std::endl;
    std::cout << "[image1].jpg [image2].jpg [image3].jpg [list1].list [list2].list [list3].list : \t launch the program with the three given images and the list of corresponding points" << std::endl << std::endl;
    std::cout << "[image1].jpg [image2].jpg [image3].jpg [list1].list [list2].list [list3].list [repository]: \t launch the program with the three given images and the list of corresponding points and the path to the repository where the list of cliked points will be saved" << std::endl << std::endl;

  }

  // If there is not at least 3 arguments, we load the default pictures and their lists
  if(argc <= 2){

    images[0] = IMG_Load("input/image1.jpg");
    images[1] = IMG_Load("input/image2.jpg");
    images[2] = IMG_Load("input/image3.jpg");

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    kn::loadMatrix(list1,"input/list1.list");
    kn::loadMatrix(list2,"input/list2.list");
    kn::loadMatrix(list3,"input/list3.list");

    return EXIT_SUCCESS;
  }
  // If there is the three image files
  else if(argc == 4){

    images[0] = IMG_Load(argv[1]);
    images[1] = IMG_Load(argv[2]);
    images[2] = IMG_Load(argv[3]);

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    return EXIT_SUCCESS;
  }

  else if(argc == 5){

    repository = argv[4];

    images[0] = IMG_Load(argv[1]);
    images[1] = IMG_Load(argv[2]);
    images[2] = IMG_Load(argv[3]);

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    return EXIT_SUCCESS;
  }

  // If the images and lists files are mentionned, there are loaded
  else if(argc == 7){

    images[0] = IMG_Load(argv[1]);
    images[1] = IMG_Load(argv[2]);
    images[2] = IMG_Load(argv[3]);

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    kn::loadMatrix(list1,argv[4]);
    kn::loadMatrix(list2,argv[5]);
    kn::loadMatrix(list3,argv[6]);

    return EXIT_SUCCESS;
   }

   else if(argc == 8){

    repository = argv[7];

    images[0] = IMG_Load(argv[1]);
    images[1] = IMG_Load(argv[2]);
    images[2] = IMG_Load(argv[3]);

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    kn::loadMatrix(list1,argv[4]);
    kn::loadMatrix(list2,argv[5]);
    kn::loadMatrix(list3,argv[6]);

    return EXIT_SUCCESS;
   }

  std::cerr << "Invalid argument, please type -h to read the help" << std::endl;
  return EXIT_FAILURE;
}


void calculateTensor(Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3, Eigen::VectorXd& t, Tensor& tensor, Eigen::MatrixXd& A) {
    // Calculation of the matrix A in At = 0
    if(list1.rows() >= 7 && list2.rows() >= 7 && list3.rows() >= 7) {
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
}

void firstTransfert(Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3, Tensor& tensor) {
    MatrixXd B = MatrixXd::Zero(4,2);
    VectorXd x = VectorXd::Zero(2);
    VectorXd b = VectorXd::Zero(4);

    // Calculation of the matrix B in Bx = 0 for the transfert on the first images
    if( (list2.rows() > 7 && list3.rows() >7) && (list2.rows()==list3.rows() && list1.rows() == list2.rows()-1 )) {
        std::cout << "Transfert on the first picture" <<std::endl;
        for(int i = 0; i<2; ++i) {
            for(int j = 0; j<2; ++j) {
                //for(int k = 0; k<3; ++k) {
                    B(2*i + j, 0) = list2(list2.rows()-1,i)*tensor(2,j,0) - tensor(i,j,0) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,0) + list3(list2.rows()-1,j)*tensor(i,2,0);
                    B(2*i + j, 1) = list2(list2.rows()-1,i)*tensor(2,j,1) - tensor(i,j,1) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,1) + list3(list2.rows()-1,j)*tensor(i,2,1);
                    b(2*i + j) = - (list2(list2.rows()-1,i)*tensor(2,j,2) - tensor(i,j,2) - list2(list2.rows()-1,i)*list3(list2.rows()-1,j)*tensor(2,2,2) + list3(list2.rows()-1,j)*tensor(i,2,2));
                //}
            }
        }           

        // Apply the SVD
        Eigen::JacobiSVD<MatrixXd> jacobiB;
        jacobiB.compute(B, ComputeThinU | ComputeThinV);
        x = jacobiB.solve(b);

        // Add the point to list1
        updateMatrix( list1, x(0), x(1), 1.0, "/tmp/myList1.mat");
    }    
}



void secondTransfert(Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3, Tensor& tensor) {
    MatrixXd B = MatrixXd::Zero(4,2);
    VectorXd x = VectorXd::Zero(2);
    VectorXd b = VectorXd::Zero(4);

    // Calculation of the matrix B in Bx = 0 for the transfert on the second images
          if( (list1.rows() > 7 && list3.rows() >7) && (list1.rows()==list3.rows() && list2.rows() == list1.rows()-1 )) {
            std::cout << "Transfert on the second picture" <<std::endl;

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
}

void thirdTransfert(Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3, Tensor& tensor) {
    MatrixXd B = MatrixXd::Zero(4,2);
    VectorXd x = VectorXd::Zero(2);
    VectorXd b = VectorXd::Zero(4);

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


    