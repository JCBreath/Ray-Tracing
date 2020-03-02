
#include "AreaLight.h"

AreaLight::AreaLight(const Point& position, const Color& color, const double& r)
  : position(position), color(color), r(r)
{
}

AreaLight::~AreaLight()
{
}

void AreaLight::preprocess()
{
}

double AreaLight::getLight(Color& light_color, Vector& light_direction,
                            const RenderContext&, const Point& hitpos) const
{
  light_color = color;

  Vector dir = position-hitpos;
  double phi = (double)rand()/RAND_MAX * M_PI * 2;
  double r_r = (2.*(double)rand()/RAND_MAX - 1) * r;
  Vector r_u=Cross(dir,Vector(0,1.,0)) * cos(phi);
  Vector r_v=Cross(dir,r_u) * sin(phi);
  Vector shift = r_u + r_v;
  shift.normalize();

  Point rand_point = position + shift * r_r;
  dir = rand_point - hitpos;
  
  double len = dir.normalize();
  light_direction = dir;
  return len;
}
