
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

  Color direct_light(0,0,0);
  direct_light += scene->getAmbient()*Ka;

  Color indirect_light(0,0,0);
 /*
  Color light = scene->getAmbient()*Ka;
  Color spec_light(0,0,0);

  Color refl_color;
  Vector refl_direction = 2 * Dot(normal, -ray.direction()) * normal - (-ray.direction());
  refl_direction.normalize();



  Ray refl_ray(hitpos, refl_direction);
  // Ray refl_ray(hitpos, final_dir);
  scene->traceRay(refl_color, context, refl_ray, atten * .5, depth + 1);



  Color indir_color;
  Vector indir_direction = 2 * Dot(normal, -ray.direction()) * normal - (-ray.direction());
  indir_direction.normalize();

  double theta = (double)rand()/RAND_MAX * M_PI / 2;
  double phi = (double)rand()/RAND_MAX * M_PI * 2;
  
  Vector r_u=Cross(normal,Vector(0,1.,0)) * sin(theta) * cos(phi);
  Vector r_v=Cross(normal,r_u) * sin(theta) * sin(phi);
  Vector final_dir = normal * cos(theta) + r_u + r_v;
  Ray indir_ray(hitpos, final_dir);
  scene->traceRay(indir_color, context, indir_ray, atten * .5, depth + 1);

  indir_color *= Kd*Dot(normal, final_dir);
*/
// DIRECT ILLUMINATION
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
    double cos_spec_phi = -Dot(ray.direction(), refl_direction);

    

      // Cast shadow rays...
      HitRecord shadowhit(dist);
      Ray shadowray(hitpos, light_direction);
      world->intersect(shadowhit, context, shadowray, (double)rand()/RAND_MAX*(scene->getEnd()-scene->getStart()) + scene->getStart());
      if(!shadowhit.getPrimitive()) {
        // No shadows...
        if(cosphi > 0){
          direct_light += light_color*(Kd*cosphi);
        }
        if(cos_spec_phi > 0){
          direct_light += light_color*(Ks*pow(cos_spec_phi, 10));
        }  
      }
  }
  
  // MIRROR
  Color mirror_color(0,0,0);
  /*
  Vector eye_refl_direction = 2 * Dot(normal, -ray.direction()) * normal - (-ray.direction());
  Ray mirror_ray(hitpos, eye_refl_direction);
  scene->traceRay(mirror_color, context, mirror_ray, atten*.5, depth + 1.);
  mirror_color *= Ks;
  */
  /*
  // INDIRECT ILLUMINATION
  for(int i=0;i<1;i++) {
    Color indirect_light_color(0,0,0);

    // double phi = (double)rand()/RAND_MAX * M_PI / 2.;
    // double theta = (double)rand()/RAND_MAX * M_PI * 2.;
    // Vector r_u=Cross(normal,Vector(0.,1.,0.)) * sin(theta) * cos(phi);
    // Vector r_v=Cross(normal,r_u) * sin(theta) * sin(phi);
    // Vector indirect_dir = normal * cos(theta) + r_u + r_v;


    Vector r_u=Cross(normal,Vector(1.,0,0));
    r_u.normalize();
    Vector r_v=Cross(normal,r_u);
    r_v.normalize();
    Point rand_point = hitpos + normal + r_u*(2.*(double)rand()/RAND_MAX - 1) + r_v*(2.*(double)rand()/RAND_MAX - 1);
    Vector indirect_dir = rand_point - hitpos;
    indirect_dir.normalize();
    
    Ray indir_ray(hitpos, indirect_dir);
    scene->traceRay(indirect_light_color, context, indir_ray, atten*.5, depth + 1.);
    double cos_indirect_diffuse_phi = Dot(normal, indirect_dir);
    Vector indir_refl_direction = 2 * Dot(normal, indirect_dir) * normal - indirect_dir;
    indir_refl_direction.normalize();
    double cos_indirect_specular_phi = -Dot(ray.direction(), indir_refl_direction);


    // BRDF
    if(cos_indirect_diffuse_phi > 0){
      indirect_light += indirect_light_color * atten *(Kd*cos_indirect_diffuse_phi);
    }
    if(cos_indirect_specular_phi > 0){
      indirect_light += indirect_light_color * atten *(Ks*cos_indirect_specular_phi);
    }
    

    
  }
  indirect_light /= 1.;
  */

  // result = (light) * color +  refl_color * Ks * atten;
  result = direct_light * color + indirect_light * color + mirror_color;
  // result = (light + indir_color) * color +  refl_color * Ks * atten;
  //result = light*color + refl_color*Ks*atten;
}


