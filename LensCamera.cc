
#include "LensCamera.h"
#include "Ray.h"
#include "Math.h"
#include <math.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

LensCamera::LensCamera(const Point& eye, const Point& lookat, const Vector& up,
                             double hfov, double focal_length, double aperture)
  : eye(eye), lookat(lookat), up(up), hfov(hfov), focal_length(focal_length)
{
  r = focal_length / aperture;
}

LensCamera::~LensCamera()
{
}

void LensCamera::preprocess(double aspect_ratio)
{
  lookdir = lookat-eye;
  lookdir.normalize();
  u = Cross(lookdir, up);
  v = Cross(u, lookdir);
  double ulen = tan(hfov/2.*M_PI/180.);
  u.normalize();
  u *= ulen;
  double vlen = ulen/aspect_ratio;
  v.normalize(); 
  v *= vlen;
}
/*
void LensCamera::makeRay(Ray& ray, const RenderContext& context, double x, double y) const
{
  Vector direction = lookdir+u*x+v*y;
  direction.normalize();
  ray = Ray(eye, direction);
}
*/
void LensCamera::makeRay(Ray& ray, const RenderContext& context, double x, double y) const
{
  Vector direction = lookdir+u*x+v*y;
  direction.normalize();
  
  Ray center_ray(eye, direction);
  Vector ray_direction = center_ray.direction();
  
  ray_direction.normalize();
  // cout<<direction<<" "<<ray_direction<<endl;

  Point focal_point = center_ray.origin() + focal_length * center_ray.direction();
  Vector r_u=Cross(lookdir,Vector(r,0,0));
  Vector r_v=Cross(lookdir,r_u);

  Point rand_point = eye + r_u*(2.*(double)rand()/RAND_MAX - 1) + r_v*(2.*(double)rand()/RAND_MAX - 1);
  // cout<<eye<<" "<<rand_point<<endl;
  Vector final_dir = focal_point - rand_point;
  final_dir.normalize();
  ray = Ray(rand_point, final_dir);
  //ray = Ray(rand_point, direction);
}
