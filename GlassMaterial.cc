
#include "GlassMaterial.h"
#include "HitRecord.h"
#include "Light.h"
#include "Point.h"
#include "Primitive.h"
#include "Ray.h"
#include "RenderContext.h"
#include "Scene.h"
#include "Background.h"
#include "Vector.h"
#include "Math.h"
using namespace std;
#include <iostream>


GlassMaterial::GlassMaterial()
{
  n_glass = 1.55;
}

GlassMaterial::~GlassMaterial()
{
}

void GlassMaterial::shade(Color& result, const RenderContext& context,
                               const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
  const Scene* scene = context.getScene();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
  double costheta = Dot(normal, ray.direction());
  bool inside = false;
  if(costheta > 0) {
    normal = -normal;
    inside = true;
  }

  const Object* world = scene->getObject();

  float facingratio = -Dot(ray.direction(), normal); 
  float fresneleffect = 1 - facingratio;
  Vector refldir = ray.direction() - normal * 2 * Dot(ray.direction(), normal); 
  refldir.normalize();
  double cos_spec_phi = Dot(-ray.direction(), refldir);
  Color refl_color;
  Ray refl_ray(hitpos, refldir);
  scene->traceRay(refl_color, context, refl_ray, atten*.5, depth + 1.);
  float ior = n_glass, eta = (inside) ? ior : 1 / ior;
  float cosi = Dot(-normal, ray.direction()); 
  float k = 1 - eta * eta * (1 - cosi * cosi); 
  Vector refrdir = ray.direction() * eta + normal * (eta *  cosi - sqrt(k)); 
  refrdir.normalize();
  Color refra_color;
  Ray refra_ray(hitpos, refrdir);
  scene->traceRay(refra_color, context, refra_ray, atten*.5, depth + 1.);
  // fresneleffect = 0.;
  result = refl_color * (fresneleffect) + refra_color * (1 - fresneleffect);
}


void GlassMaterial::photon(Color& light, const RenderContext& context, const Ray& ray,
  const HitRecord& hit, Color& power, Point& pos, Vector& dir) const
{
  const Scene* scene = context.getScene();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
  double costheta = Dot(normal, ray.direction());
  bool inside = false;
  if(costheta > 0) {
    normal = -normal;
    inside = true;
  }

  float facingratio = -Dot(ray.direction(), normal); 
  float fresneleffect = 1 - facingratio;
  double r = (double)rand()/RAND_MAX;
  if(r < fresneleffect) {
  // if(false) {
    Vector refldir = ray.direction() - normal * 2 * Dot(ray.direction(), normal); 
    refldir.normalize();
    Color refl_color;
    Ray refl_ray(hitpos, refldir);
    scene->tracePhoton(light, context, refl_ray, power, pos, dir);
  } else {
    float n_fac = 1.;
    if(power.r() > 0.) {
      // cout<<power<<endl;
      n_fac = 0.95;
    } else if (power.b() > 0.) {
      n_fac = 1.05;
    }
    float ior = n_glass * n_fac, eta = (inside) ? ior : 1 / ior;
    float cosi = Dot(-normal, ray.direction()); 
    float k = 1 - eta * eta * (1 - cosi * cosi); 
    Vector refrdir = ray.direction() * eta + normal * (eta *  cosi - sqrt(k)); 
    refrdir.normalize();
    Color refra_color;
    Ray refra_ray(hitpos, refrdir);
    scene->tracePhoton(refra_color, context, refra_ray, power, pos, dir);
  }
  // Ray refra_ray(hitpos, ray.direction());
  // scene->tracePhoton(light, context, refra_ray, power, pos, dir);
  // light = refl_color * fresneleffect + refra_color * (1 - fresneleffect);
}
