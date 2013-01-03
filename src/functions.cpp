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

bool readArguments(int argc, char** argv, SDL_Surface** images, Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3){

  // English help
  if( (argc == 2) && (strcmp("-h",argv[1]) == 0)) {
    std::cout << "Help in english" << std::endl;
  }
  // If there is not at least 3 arguments, we load the default pictures and their lists
  else if(argc <= 2){

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

  std::cerr << "Invalid argument, please type -h to read the help" << std::endl;
  return EXIT_FAILURE;
}