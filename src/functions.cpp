#include "functions.hpp"

void updateMatrix(MatrixXd &list, float newX, float newY, float newZ, const std::string &filename) {
  MatrixXd tmp = list;
  list.resize(list.rows() + 1, list.cols());
  list << tmp, newX, newY, newZ;
  kn::saveMatrix(list, filename);
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
    std::ofstream list1File;
    kn::loadMatrix(list2,"input/list2.list");
    std::ofstream list2File;
    kn::loadMatrix(list3,"input/list3.list");
    std::ofstream list3File;

    return EXIT_SUCCESS;
  }
  // If there is the three image files, there are loaded with empty lists
  else if(argc == 4){

    images[0] = IMG_Load(argv[1]);
    images[1] = IMG_Load(argv[2]);
    images[2] = IMG_Load(argv[3]);

    if(images[0] == 0 || images[1] == 0 || images[2] == 0){
      std::cerr << "error loading images" << std::endl;
    }

    kn::loadMatrix(list1,"input/emptyList.list");
    std::ofstream list1File;
    kn::loadMatrix(list2,"input/emptyList.list");
    std::ofstream list2File;
    kn::loadMatrix(list3,"input/emptyList.list");
    std::ofstream list3File;

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
    std::ofstream list1File;
    kn::loadMatrix(list2,argv[5]);
    std::ofstream list2File;
    kn::loadMatrix(list3,argv[6]);
    std::ofstream list3File;

    return EXIT_SUCCESS;
   }

  std::cerr << "Invalid argument, please type -h to read the help" << std::endl;
  return EXIT_FAILURE;
}