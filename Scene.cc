
#include "Scene.h"
#include "Background.h"
#include "Camera.h"
#include "HitRecord.h"
#include "Image.h"
#include "Light.h"
#include "Material.h"
#include "GlassMaterial.h"
#include "Object.h"
#include "Ray.h"
#include "RenderContext.h"
#include <float.h>
#include <iostream>
#include <stdlib.h>
#include <vector>
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
  photon_count = 0;
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
  setMaxRayDepth(4);
  RenderContext context(this);
  double dx = 2./xres;
  double xmin = -1. + dx/2.;
  double dy = 2./yres;
  double ymin = -1. + dy/2.;
  Color atten(1.,1.,1.);

  // PHOTON MAPPING
  Point light_pos(-400.0, -104.0, 4.0);
  Point obj_pos(-20.0, -5.02, 0.0);
  double r = 0.5;
  Vector l_dir = light_pos - obj_pos;
  l_dir.normalize();
  for(int i=0;i<500000;i++){
    Color l;
    double r1 = (double)rand()/RAND_MAX;
    double r2 = (double)rand()/RAND_MAX - .5;
    double phi = M_PI * 2. * r1;
    phi = 2 * i * M_PI / 1000;
    double r_r = r * sqrt(r2);
    Vector r_du = Cross(l_dir,Vector(0,1.,0));
    r_du.normalize();
    Vector r_dv = Cross(l_dir,r_du);
    r_dv.normalize();
    Vector r_u= r_du * cos(phi);
    Vector r_v= r_dv * sin(phi);
    Vector shift = r_u + r_v;
    
    Point rand_point = obj_pos + shift * r_r;
    // Point rand_point = obj_pos + Vector(1,0,0)*r1*5 + Vector(0,1,0)*r2*2;
    // rand_point = obj_pos;
    Vector rand_dir = rand_point - light_pos;
    rand_dir.normalize();

    Ray ph_ray(light_pos, rand_dir);
    Color power(.98, 1., .88);
    double clr_r = (double)rand()/RAND_MAX * 3;
    if(clr_r < 1.){
      power = Color(.98, 0., 0.);
    } else if(clr_r < 2.){
      power = Color(0., 1., 0.);
    } else {
      power = Color(0., 0., 0.88);
    }
    
    Point pos;
    Vector dir;
    tracePhoton(l, context, ph_ray, power, pos, dir);
    Photon *p = new Photon();
    p->pos = pos;
    p->dir = dir;
    p->color = power;
    // cout<<p->pos<<endl;
    // cout<<(int)round(p->pos.x())<<endl;
    int p_x = (int)round(p->pos.x());
    int p_y = (int)round(p->pos.y());
    if(p_x >= -24 && p_x < 25 && p_y >= -24 && p_y < 25) {
      photon_map[p_x+24][p_y+24].push_back(p);
      photon_count++;
    }
  }
  // cout<<photon_map[24][24].size()<<endl;
  // RAY TRACING
  
  for(int i=0;i<yres;i++){
    //cerr << "y=" << i << '\n';
    double y = ymin + i*dy;
    #pragma omp parallel for schedule(dynamic, 1)
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
    cout<<(float)i/yres*100.<<"%"<<endl;
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


double Scene::tracePhoton(Color& result, const RenderContext& context, const Ray& ray,  Color& power, Point& pos, Vector& dir) const
{
  HitRecord hit(DBL_MAX);
  object->intersect(hit, context, ray, 0);
  if(hit.getPrimitive()){
    // Ray hit something...
    const Material* matl = hit.getMaterial();
    matl->photon(result, context, ray, hit, power, pos, dir);
    return hit.minT();
  } else {
    return DBL_MAX;
  }
}

Color Scene::getRadiance(Point p) const
{
  
  Color c(0,0,0);
  
  int p_x = (int)round(p.x()) + 24;
  int p_y = (int)round(p.y()) + 24;
  if(!(p_x >= 0 && p_x < 50 && p_y >= 0 && p_y < 50)) {
    return c;
  }
  vector<Photon*> p_map = photon_map[p_x][p_y];
  
  // if(p_map.size() == 0)
  //   cout<<p_map.size()<<endl;
  
  
  double radius = .5;
  // #pragma omp parallel for schedule(dynamic, 1)
  for(int j=0;j<p_map.size();j++){
    double dist = (p - p_map[j]->pos).normalize();
    if(dist < radius) {
      c += p_map[j]->color * (double)(1. / photon_count / radius / radius / dist);
    }
  }
  
  if (abs(p.x()+24 - p_x) < radius) {
    bool valid = false;  
    if(p.x()+24 - p_x < 0 && p_x >= 1) {
      p_map = photon_map[p_x-1][p_y];
      valid = true;
    }
    else if(p.x()+24 - p_x > 0 && p_x < 49){
      p_map = photon_map[p_x+1][p_y];
      valid = true;
    }
    if(valid) {
      for(int j=0;j<p_map.size();j++){
        double dist = (p - p_map[j]->pos).normalize();
        if(dist < radius) {
          c += p_map[j]->color * (double)(1. / photon_count / radius / radius / dist);
        }
      }
    }
  }

  if (abs(p.y()+24 - p_y) < radius) {
    bool valid = false;  
    if(p.y()+24 - p_y < 0 && p_y >= 1) {
      p_map = photon_map[p_x][p_y-1];
      valid = true;
    }
    else if(p.y()+24 - p_y > 0 && p_y < 49){
      p_map = photon_map[p_x][p_y+1];
      valid = true;
    }
    if(valid) {
      for(int j=0;j<p_map.size();j++){
        double dist = (p - p_map[j]->pos).normalize();
        if(dist < radius) {
          c += p_map[j]->color * (double)(1. / photon_count / radius / radius / dist);
        }
      }
    }
  }

  if (abs(p.x()+24 - p_x) < radius && abs(p.y()+24 - p_y) < radius) {
    bool valid = false;  
    if(p.x()+24 - p_x < 0 && p_x >= 1 && p.y()+24 - p_y < 0 && p_y >= 1) {
      p_map = photon_map[p_x-1][p_y-1];
      valid = true;
    }
    else if(p.x()+24 - p_x > 0 && p_x < 49 && p.y()+24 - p_y > 0 && p_y < 49){
      p_map = photon_map[p_x+1][p_y+1];
      valid = true;
    }
    else if(p.x()+24 - p_x > 0 && p_x < 49 && p.y()+24 - p_y < 0 && p_y >= 1){
      p_map = photon_map[p_x+1][p_y-1];
      valid = true;
    }
    else if(p.x()+24 - p_x < 0 && p_x >= 1  && p.y()+24 - p_y > 0 && p_y < 49){
      p_map = photon_map[p_x-1][p_y+1];
      valid = true;
    }
    if(valid) {
      for(int j=0;j<p_map.size();j++){
        double dist = (p - p_map[j]->pos).normalize();
        if(dist < radius) {
          c += p_map[j]->color * (double)(1. / photon_count / radius / radius / dist);
        }
      }
    }
  }


  /*
  if (abs(p.z()+24 - p_y) < radius && abs(p.y()+24 - p_x) < radius) {
    if(p.z()+24 - p_y > 0 && p.y()+24 - p_x > 0)
      p_map = photon_map[p_x-1][p_y-1];
    else if(p.z()+24 - p_y < 0 && p.y()+24 - p_x > 0)
      p_map = photon_map[p_x-1][p_y+1];
    else if(p.z()+24 - p_y > 0 && p.y()+24 - p_x < 0)
      p_map = photon_map[p_x+1][p_y-1];
    else if(p.z()+24 - p_y < 0 && p.y()+24 - p_x < 0)
      p_map = photon_map[p_x+1][p_y+1];
    for(int j=0;j<p_map.size();j++){
      double dist = (p - p_map[j]->pos).normalize();
      if(dist < radius) {
        c += p_map[j]->color * (double)(1. / p_map.size() / radius / radius);
      }
    }
  }
  */
  c = c * 1. * 3.;
  return(c);
  
}