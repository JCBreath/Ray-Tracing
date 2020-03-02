
#include "Scene.h"
#include "Background.h"
#include "Camera.h"
#include "HitRecord.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "RenderContext.h"
#include <float.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

Scene::Scene()
{
  object = 0;
  background = 0;
  camera = 0;
  ambient = Color(0, 0, 0);
  image = 0;
  minAttenuation = 0;
  ssaa = true;
  supersamples = 4;
  dof_s = 4;
  start = 0.0;
  end = 1.0;
}

Scene::~Scene()
{
  delete object;
  delete background;
  delete camera;
  delete image;
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    delete light;
  }
}

double Scene::getTime(double start, double end) {
  return (double)rand()/RAND_MAX*(end-start) + start;
}

void Scene::preprocess()
{
  background->preprocess();
  for(int i=0;i<static_cast<int>(lights.size());i++){
    Light* light = lights[i];
    light->preprocess();
  }
  double aspect_ratio = image->aspect_ratio();
  camera->preprocess(aspect_ratio);
  object->preprocess();
}

void Scene::render()
{
  if(!object || !background || !camera || !image){
    cerr << "Incomplete scene, cannot render!\n";
    exit(1);
  }
  int xres = image->getXresolution();
  int yres = image->getYresolution();
  setMaxRayDepth(3);
  RenderContext context(this);
  double dx = 2./xres;
  double xmin = -1. + dx/2.;
  double dy = 2./yres;
  double ymin = -1. + dy/2.;
  Color atten(1.,1.,1.);
  for(int i=0;i<yres;i++){
    //cerr << "y=" << i << '\n';
    double y = ymin + i*dy;
    for(int j=0;j<xres;j++){
      double x = xmin + j*dx;
      //cerr << "x=" << j << ", y=" << i << '\n';
      Color result(0, 0, 0);
      if(ssaa){
        for(int k=0;k<supersamples;k++){
          for(int l=0;l<supersamples;l++){
            Ray ray;
            double xx, yy;
            double x_rand, y_rand;
            x_rand = (double)rand()/RAND_MAX/supersamples + (double)l/supersamples;
            y_rand = (double)rand()/RAND_MAX/supersamples + (double)k/supersamples;
            xx = x + x_rand * dx;
            yy = y + y_rand * dy;
          
            for(int m=0;m<dof_s;m++){
              camera->makeRay(ray, context, xx, yy);
              HitRecord hit(DBL_MAX);
              Color ss_result;
              traceRay(ss_result, context, ray, atten, 0);
              result += ss_result;
            }
            
          }
        }
        result /= (double)supersamples * supersamples * dof_s;
      }else{
        Ray ray;
        camera->makeRay(ray, context, x, y);
        HitRecord hit(DBL_MAX);
        traceRay(result, context, ray, atten, 0);
      }
      
      
      
      // camera->makeRay(ray, context, x, y);
      
      
      image->set(j, i, result);
    }
  }
}

double Scene::traceRay(Color& result, const RenderContext& context, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    object->intersect(hit, context, ray, (double)rand()/RAND_MAX*(end-start) + start);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}

double Scene::traceRay(Color& result, const RenderContext& context, const Object* obj, const Ray& ray, const Color& atten, int depth) const
{
  if(depth >= maxRayDepth || atten.maxComponent() < minAttenuation){
    result = Color(0, 0, 0);
    return 0;
  } else {
    HitRecord hit(DBL_MAX);
    obj->intersect(hit, context, ray, (double)rand()/RAND_MAX*(end-start) + start);
    if(hit.getPrimitive()){
      // Ray hit something...
      const Material* matl = hit.getMaterial();
      matl->shade(result, context, ray, hit, atten, depth);
      return hit.minT();
    } else {
      background->getBackgroundColor(result, context, ray);
      return DBL_MAX;
    }
  }
}
