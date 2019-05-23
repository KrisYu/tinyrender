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
  ViewPort[2][3] = 255/2.f;
  ViewPort[0][0] = w/2.f;
  ViewPort[1][1] = h/2.f;
  ViewPort[2][2] = 255/2.f;
}

void projection(float coeff) {
  Projection = Matrix::identity();
  Projection[3][2] = coeff;
}

Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
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

void triangle(Vec4f *pts, TGAImage &image, TGAImage &zbuffer, TGAColor color) {
  Vec2f bboxmin( std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  Vec2f bboxmax(-std::numeric_limits<float>::max(),-std::numeric_limits<float>::max());
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 2; j++) {
      // x/w y/w
      bboxmin[j] = std::min(bboxmin[j], pts[i][j]/pts[i][3]);
      bboxmax[j] = std::max(bboxmax[j], pts[i][j]/pts[i][3]);
    }
  }

  Vec2i P;
  for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
    for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
      Vec3f c = barycentric(proj<2>(pts[0]/pts[0][3]), proj<2>(pts[1]/pts[1][3]),
       proj<2>(pts[2]/pts[2][3]), P);
      float z = pts[0][2]*c.x + pts[1][2]*c.y + pts[2][2]*c.z;
      float w = pts[0][3]*c.x + pts[1][3]*c.y + pts[2][3]*c.z;
      int frag_depth = std::max(0, std::min(255, int(z/w+.5)));
      if (c.x < 0 || c.y < 0 || c.z < 0 || zbuffer.get(P.x, P.y)[0] > frag_depth ) continue;
      image.set(P.x, P.y, color);
      zbuffer.set(P.x, P.y, TGAColor(frag_depth));
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

  lookat(eye, center, up);
  viewport(width/8,  height/8, width*3/4, height*3/4);
  projection(-1.f/3);

  TGAImage image(width, height, TGAImage::RGB);
  TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

  for (int i = 0; i < model->nfaces(); i++) {
    std::vector<int> face = model->face(i);
    Vec3f world_coords[3];
    Vec4f screen_coords[3];
    for (int j = 0; j < 3; j++) {
      world_coords[j] = model->vert(face[j]);
      screen_coords[j] = world2screen(world_coords[j]);
    }

    Vec3f norm = cross(world_coords[2] - world_coords[0], world_coords[1] - world_coords[0]);
    norm.normalize();
    float intensity = light_dir*norm;
    if (intensity > 0) {
      triangle(screen_coords, image, zbuffer, TGAColor(intensity*255,intensity*255,intensity*255,255));
    }
  }

  image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
  zbuffer.flip_vertically();
  image.write_tga_file("output.tga");
  zbuffer.write_tga_file("zbuffer.tga");

  delete model;
  return 0;
}
