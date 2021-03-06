
#ifndef LambertianMaterial_h
#define LambertianMaterial_h

#include "Material.h"
#include "Color.h"

class LambertianMaterial : public Material {
 public:
  LambertianMaterial(const Color& color, float Kd, float Ka);
  virtual ~LambertianMaterial();

  virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, const Color& atten, int depth) const;
  virtual void photon(Color& light, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, Color& power, Point& pos, Vector& dir) const;

 private:
  LambertianMaterial(const LambertianMaterial&);
  LambertianMaterial& operator=(const LambertianMaterial&);

  Color color;
  float Kd;
  float Ka;
};

#endif
