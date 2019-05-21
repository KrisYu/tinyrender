#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <string>
#include "geometry.h"
class Model {
private:
  std::vector<Vec3f> verts_;
public:
  Model(const char *filename);
  ~Model();
  Vec3f vert(int i);
  int nverts();
};

#endif
