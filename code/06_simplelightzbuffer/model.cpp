#include <iostream>
#include <fstream>
#include <sstream>

#include "model.h"

Model::Model(const char *filename): verts_(), faces_()
{
  std::ifstream in(filename, std::ifstream::in);
  if (in.is_open()) {
    std::string line;
    while (std::getline(in,line))
    {
      char trash;
      std::istringstream iss(line);
      if(line.substr(0,2) == "v "){
        iss >> trash;
        Vec3f v;
        for (int i = 0; i < 3; i++) iss >> v[i];
        verts_.push_back(v);
      } else if (line.substr(0,2) == "f "){
        iss >> trash;
        std::vector<int> f;
        int idx,itrash;
        while (iss >> idx >> trash >> itrash >> trash >> itrash) {
          idx--; // in wavefront obj all indices start at 1, not zero
          f.push_back(idx);
        }
        faces_.push_back(f);
      }
    }

    in.close();
  }

  std::cout << "# v#" << verts_.size() << std::endl;

}

Model::~Model(){}

int Model::nverts(){
  return (int)verts_.size();
}

Vec3f Model::vert(int i){
  return verts_[i];
}

int Model::nfaces(){
  return (int)faces_.size();
}

std::vector<int> Model::face(int idx){
  return faces_[idx];
}
