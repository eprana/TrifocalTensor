#ifndef _HPP_FUNCTIONS_
#define _HPP_FUNCTIONS_

#include <iostream>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <Eigen/Geometry>


// Read the arguments and load the right items
bool readArguments(int argc, char** argv, SDL_Surface** images, Eigen::MatrixXd& list1, Eigen::MatrixXd& list2, Eigen::MatrixXd& list3);

// Update a matrix 
void updateMatrix(MatrixXd &list, float newX, float newY, float newZ, const std::string &filename);
#endif