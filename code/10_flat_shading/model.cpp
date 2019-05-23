#include <iostream>
#include <fstream>
#include <sstream>

#include "model.h"

Model::Model(const char *filename): verts_(), faces_(), norms_(), uv_()
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
      } else if (line.substr(0,2) == "vn") {
        iss >> trash >> trash;
        Vec3f n;
        for (int i = 0; i < 3; i++) iss >> n[i];
        norms_.push_back(n);
      }else if (line.substr(0,2)=="vt") {
        iss >> trash >> trash;
        Vec2f uv;
        for (int i = 0; i < 2; i++) iss >> uv[i];
        uv_.push_back(uv);
      }else if (line.substr(0,2) == "f "){
        std::vector<Vec3i> f;
        Vec3i tmp;
        iss >> trash;
        while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
          for (int i = 0; i < 3; i++) tmp[i]--; // in wavefront obj all indices start at 1, not zero
          f.push_back(tmp);
        }
        faces_.push_back(f);
      }
    }
    in.close();
  }

  std::cerr << "# v#" << verts_.size() <<" f# " << faces_.size() <<" vt# "<<
  uv_.size() << " vn# " << norms_.size() <<  std::endl;
}

Model::~Model(){}

int Model::nverts(){
  return (int)verts_.size();
}

int Model::nfaces(){
  return (int)faces_.size();
}

std::vector<int> Model::face(int idx){
  std::vector<int> face;
  for (int i = 0; i < (int)faces_[idx].size(); i++) face.push_back(faces_[idx][i][0]);
  return face;
}

Vec3f Model::vert(int iface, int nthvert){
  return verts_[faces_[iface][nthvert][0]];
}

Vec2f Model::uv(int iface, int nthvert){
  return uv_[faces_[iface][nthvert][1]];
}

Vec3f Model::normal(int iface, int nthvert){
  int idx = faces_[iface][nthvert][2];
  return norms_[idx].normalize();
}
