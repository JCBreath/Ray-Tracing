
#ifndef GlassMaterial_h
#define GlassMaterial_h

#include "Material.h"
#include "Color.h"

class GlassMaterial : public Material {
 public:
  GlassMaterial();
  virtual ~GlassMaterial();

  virtual void shade(Color& result, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, const Color& atten, int depth ) const;
  virtual void photon(Color& light, const RenderContext& context, const Ray& ray,
                     const HitRecord& hit, Color& power, Point& pos, Vector& dir) const;

 private:
  GlassMaterial(const GlassMaterial&);
  GlassMaterial& operator=(const GlassMaterial&);

  float n_glass;
};

#endif
