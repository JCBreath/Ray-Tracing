
#include "PhongMaterial.h"
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


PhongMaterial::PhongMaterial(const Color& color, float Kd, float Ka, float Ks)
  :color(color), Kd(Kd), Ka(Ka), Ks(Ks)
{
  shininess = 1;
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::shade(Color& result, const RenderContext& context,
                               const Ray& ray, const HitRecord& hit, const Color& atten, int depth) const
{
  const Scene* scene = context.getScene();
  const vector<Light*>& lights = scene->getLights();
  Point hitpos = ray.origin()+ray.direction()*hit.minT();
  Vector normal;
  hit.getPrimitive()->normal(normal, context, hitpos, ray, hit);
  double costheta = Dot(normal, ray.direction());
  if(costheta > 0)
    normal = -normal;

  const Object* world = scene->getObject();

  Color light = scene->getAmbient()*Ka;
  Color spec_light(0,0,0);

  Color refl_color;
  Vector refl_direction = 2 * Dot(normal, -ray.direction()) * normal - (-ray.direction());
  refl_direction.normalize();
  Ray refl_ray(hitpos, refl_direction);
  scene->traceRay(refl_color, context, refl_ray, atten * .5, depth + 1);

#if 0
  for(vector<Light*>::const_iterator iter = lights.begin(); iter != lights.end(); iter++){
#else
    Light*const* begin = &lights[0];
    Light*const* end = &lights[0]+lights.size();
    while(begin != end){
#endif
    Color light_color;
    Vector light_direction;
    double dist = (*begin++)->getLight(light_color, light_direction, context, hitpos);
    double cosphi = Dot(normal, light_direction);
    Vector refl_direction = 2 * Dot(normal, light_direction) * normal - light_direction;
    refl_direction.normalize();
    double cos_spec_angle = -Dot(ray.direction(), refl_direction);


      // Cast shadow rays...
      HitRecord shadowhit(dist);
      Ray shadowray(hitpos, light_direction);
      world->intersect(shadowhit, context, shadowray, (double)rand()/RAND_MAX*(scene->getEnd()-scene->getStart()) + scene->getStart());
      if(!shadowhit.getPrimitive()) {
        // No shadows...
        if(cosphi > 0){
          light += light_color*(Kd*cosphi);
        }
        if(cos_spec_angle > 0){
          spec_light += light_color*(Ks*pow(cos_spec_angle, 100));
        }
      }
  }
  result = light*color + spec_light +  refl_color*Ks*atten;
  //result = light*color + refl_color*Ks*atten;
}


