#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include <cassert>
#include <cmath>

template <size_t DIM, typename T> struct vec {
  vec() {for (size_t i = DIM; i--; data_[i] = T());}
        T& operator[](const size_t i)       {assert(i<DIM); return data_[i];}
  const T& operator[](const size_t i) const {assert(i<DIM); return data_[i];}

private:
  T data_[DIM];
};

template <typename T> struct vec<2,T>{
  vec() : x(T()), y(T()) {}
  vec(T X, T Y): x(X), y(Y) {}
  template <class U> vec<2,T>(const vec<2,U> &v);
        T& operator[](const size_t i)       {assert(i<2); return i <= 0 ? x : y;}
  const T& operator[](const size_t i) const {assert(i<2); return i <= 0 ? x : y;}

  T x, y;
};

template <typename T> struct vec<3,T>{
  vec() : x(T()), y(T()), z(T()) {}
  vec(T X, T Y, T Z): x(X), y(Y), z(Z) {}
  template <class U> vec<3,T>(const vec<3,U> &v);
        T& operator[](const size_t i)       {assert(i<3); return i <= 0 ? x : (1 == i? y : z);}
  const T& operator[](const size_t i) const {assert(i<3); return i <= 0 ? x : (1 == i? y : z);}
  float norm() {return std::sqrt(x*x+y*y+z*z);}
  vec<3, T> & normalize(T l=1) {*this = (*this)*(1/norm()); return *this;}

  T x, y, z;
};

template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, const vec<DIM,T>&v){
  for (size_t i = 0; i < DIM; i++) {
    out << v[i] << " ";
  }
  return out;
}

template <size_t DIM, typename T> vec<DIM,T> operator+(vec<DIM,T> lhs, const vec<DIM,T>& rhs){
  for (size_t i = DIM; i--; lhs[i]+=rhs[i]);
  return lhs;
}

template <size_t DIM, typename T> vec<DIM,T> operator-(vec<DIM,T> lhs, const vec<DIM,T>& rhs){
  for (size_t i = DIM; i--; lhs[i]-=rhs[i]);
  return lhs;
}

template <size_t DIM, typename T, typename U> vec<DIM,T> operator*(vec<DIM,T> lhs, const U& rhs) {
  for (size_t i = DIM; i--; lhs[i]*=rhs);
  return lhs;
}

template <size_t DIM, typename T> T operator*(const vec<DIM,T>& lhs,const vec<DIM,T>& rhs) {
  T ret = T();
  for (size_t i = DIM; i--; ret+=lhs[i]*rhs[i]);
  return ret;
}

template <typename T> vec<3,T> cross(vec<3,T> v1, vec<3,T> v2){
  return vec<3,T>(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
}


typedef vec<2, int> Vec2i;
typedef vec<3, float> Vec3f;

#endif
