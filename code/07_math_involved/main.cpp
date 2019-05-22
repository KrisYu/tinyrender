#include "tgaimage.h"
#include "model.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <limits>
using std::cout; using std::endl;

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);


Model *model = NULL;
const int width = 600;
const int height = 600;

Vec3f light_dir(0,  0, -1);
Vec3f       eye(0,  0,  3);
Vec3f    center(0,  0,  0);
Vec3f        up(0,  1,  0);

Matrix ModelView;
Matrix ViewPort;
Matrix Projection;

void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = cross(up,z).normalize();
    Vec3f y = cross(z,x).normalize();
    Matrix Minv = Matrix::identity();
    Matrix Tr   = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i];
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];
        Tr[i][3] = -center[i];
    }
    ModelView = Minv*Tr;
}

void viewport(int x, int y, int w, int h){
  ViewPort = Matrix::identity();
  ViewPort[0][3] = x + w/2.f;
  ViewPort[1][3] = y + h/2.f;
  ViewPort[2][3] = 255.f/2.f;
  ViewPort[0][0] = w/2.f;
  ViewPort[1][1] = h/2.f;
  ViewPort[2][2] = 255.f/2.f;
}

void projection(float coeff) {
  Projection = Matrix::identity();
  Projection[3][2] = coeff;
}

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

void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
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
      for (int i=0; i<3; i++) P.z += pts[i][2]*bc_screen[i];
      if (zbuffer[int(P.x+P.y*width)] <= P.z) {
        image.set(P.x, P.y, color);
        zbuffer[int(P.x+P.y*width)] = P.z;
      }
    }
  }
}

Vec4f world2screen(Vec3f v) {
  Vec4f gl_vertex = embed<4>(v); // embed Vec3f to homogenius coordinates
  gl_vertex = ViewPort * Projection * ModelView * gl_vertex; // transform it to screen coordinates
  return gl_vertex;
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

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    Vec3f world_coords[3];
    Vec4f screen_coords[3];
    for (int j = 0; j < 3; j++) {
      world_coords[j] = model->vert(face[j]);
      screen_coords[j] = world2screen(world_coords[j]);
      cout << screen_coords[j] << endl;
    }

    // Vec3f norm = cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]);
    // norm.normalize();
    // float intensity = light*norm;
    // if (intensity > 0) {
    //   triangle(screen_coords, zbuffer, image, TGAColor(intensity*255,intensity*255,intensity*255,255));
    // }
  }

  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  image.write_tga_file("output.tga");
  return 0;
}
