
#ifndef LensCamera_h
#define LensCamera_h

#include "Camera.h"
#include "Point.h"
#include "Vector.h"

class LensCamera : public Camera {
 public:
  LensCamera(const Point& eye, const Point& lookat, const Vector& up, double hfov, double focal_length, double aperture);
  virtual ~LensCamera();
  
  virtual void preprocess(double aspect_ratio);
  virtual void makeRay(Ray& ray, const RenderContext& context, double x, double y) const;
  // void makeRandomRay(Ray& ray, const RenderContext& context, double x, double y) const;

 private:
  LensCamera(const LensCamera&);
  LensCamera& operator=(const LensCamera&);

  Point eye;
  Point lookat;
  Vector up;
  double hfov;
  double focal_length;
  double r;

  Vector u;
  Vector v;
  Vector lookdir;
};

#endif

