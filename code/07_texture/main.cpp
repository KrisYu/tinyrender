#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits>
using std::cout; using std::endl;


Model *model = NULL;
const int width = 600;
const int height = 600;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
  Vec3f s[2];
  for (int i=2; i--; ) {
    s[i][0] = C[i]-A[i];
    s[i][1] = B[i]-A[i];
    s[i][2] = A[i]-P[i];
  }
  Vec3f u = cross(s[0], s[1]);
  if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
  return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f *pts, Vec2f *texts, float *zbuffer, TGAImage &image) {
  Vec2f bboxmin(std::numeric_limits<float>::max(),std::numeric_limits<float>::max());
  Vec2f bboxmax(std::numeric_limits<float>::min(),std::numeric_limits<float>::min());
  Vec2f clamp(image.get_width()-1, image.get_height()-1);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }
  Vec3f P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ) continue;
      P.z = 0;
      Vec2f Ptext(0,0);
      for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
      for (int i=0; i<3; i++) Ptext[0] += texts[i][0]*bc_screen[i];
      for (int i=0; i<3; i++) Ptext[1] += texts[i][1]*bc_screen[i];
      if (zbuffer[int(P.x+P.y*width)] <= P.z) {
        TGAColor color = model->diffuse(Ptext);
        image.set(P.x, P.y, color);
        zbuffer[int(P.x+P.y*width)] = P.z;
      }
    }
  }
}

Vec3f world2screen(Vec3f v) {
  // attention we have to change x and y to int otherwise they maybe too close for the for loop
  // we add 0.5 to rounding off
  return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}

int main(int argc, char** argv){
  if (2 == argc) {
    model = new Model(argv[1]);
  } else {
    model = new Model("obj/african_head.obj");
  }

  float *zbuffer = new float[width*height];
  for (int i=width*height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

  TGAImage image(width, height, TGAImage::RGB);
  Vec3f light(0, 0, -1);

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    Vec3f world_coords[3];
    Vec3f screen_coords[3];
    Vec2f texts[3];
    for (int j = 0; j < 3; j++) {
      world_coords[j] = model->vert(face[2*j]);
      screen_coords[j] = world2screen(world_coords[j]);
      texts[j] = model->uv(face[2*j+1]);
    }
    triangle(screen_coords, texts, zbuffer, image);
  }

  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}
