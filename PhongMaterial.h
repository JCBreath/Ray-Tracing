
#ifndef PhongMaterial_h
#define PhongMaterial_h

#include "Material.h"
#include "Color.h"

class PhongMaterial : public Material {
 public:
  PhongMaterial(const Color& color, float Kd, float Ka, float Ks);
  virtual ~PhongMaterial();

  virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, const Color& atten, int depth) const;
  
  virtual void photon(Color& light, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, Color& power, Point& pos, Vector& dir) const;

 private:
  PhongMaterial(const PhongMaterial&);
  PhongMaterial& operator=(const PhongMaterial&);

  Color color;
  float Kd;
  float Ka;
  float Ks;
  int shininess;
};

#endif
