#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"
class Model {
private:
  std::vector<Vec3f> verts_;
  std::vector<std::vector<Vec3i>> faces_; //Vec3i means vertex/uv/normal
  std::vector<Vec3f> norms_;
  std::vector<Vec2f> uv_;
  TGAImage diffusemap_;
  void load_texture(std::string filename, const char *suffix, TGAImage& img);
public:
  Model(const char *filename);
  ~Model();
  int nverts();
  int nfaces();
  Vec2f uv(int iface, int nthvert);
  Vec3f vert(int iface, int nthvert);
  Vec3f normal(int iface, int nthvert);
  std::vector<int> face(int idx);
  TGAColor diffuse(Vec2f uvf);
};

#endif
