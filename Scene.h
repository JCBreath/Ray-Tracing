
#ifndef Scene_h
#define Scene_h

#include "Color.h"
#include <string>
#include <vector>

class Background;
class Camera;
class Color;
class Image;
class Light;
class Object;
class RenderContext;
class Ray;

class Scene {
 public:
  Scene();
  virtual ~Scene();

  inline Object* getObject() const {
    return object;
  }
  void setObject(Object* obj) {
    object = obj;
  }

  inline Background* getBackground() const {
    return background;
  }
  void setBackground(Background* bg) {
    background = bg;
  }

  inline Camera* getCamera() const {
    return camera;
  }
  void setCamera(Camera* cam) {
    camera = cam;
  }

  inline Image* getImage() const {
    return image;
  }
  void setImage(Image* im) {
    image = im;
  }

  void addLight(Light* light) {
    lights.push_back(light);
  }
  const std::vector<Light*>& getLights() const {
    return lights;
  }

  Color getAmbient() const {
    return ambient;
  }
  void setAmbient(const Color& amb) {
    ambient = amb;
  }

  int getMaxRayDepth() const {
    return maxRayDepth;
  }
  void setMaxRayDepth(int rd) {
    maxRayDepth = rd;
  }
  double getMinAttenuation() const {
    return minAttenuation;
  }
  void setMinAttenuation(double atten) {
    minAttenuation = atten;
  }

  void setSS(int ss) {
    supersamples = ss;
    if(ss <= 1) {ssaa = false;}
  }

  void setDOF(int dofs) {
    dof_s = dofs;
  }
  
  void setStart(double s){
    start = s;
  }

  void setEnd(double e){
    end = e;
  }

  double getStart() const{
    return start;
  }

  double getEnd() const{
    return end;
  }
  

  void preprocess();
  void render();
  double traceRay(Color& result, const RenderContext& context, const Ray& ray, const Color& attenuation, int depth) const;
  double traceRay(Color& result, const RenderContext& context, const Object* obj, const Ray& ray, const Color& attenuation, int depth) const;
  double getTime(double start, double end);

 private:
  Scene(const Scene&);
  Scene& operator=(const Scene&);

  Background* background;
  Camera* camera;
  Color ambient;
  Image* image;
  Object* object;
  std::vector<Light*> lights;
  int maxRayDepth;
  double minAttenuation;
  bool ssaa;
  int supersamples;
  int dof_s;
  double start, end;
};

#endif
