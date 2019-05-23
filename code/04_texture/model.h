#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<int>> faces_;
  std::vector<Vec2f> uvs_;
  TGAImage diffusemap_;
  void load_texture(std::string filename, const char *suffix, TGAImage &img);
public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nfaces();
  Vec3f vert(int i);
  Vec2f uv(int i);
  std::vector<int> face(int idx);
  TGAColor diffuse(Vec2f uv);
};

#endif
