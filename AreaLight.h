
#ifndef AreaLight_h
#define AreaLight_h

#include "Light.h"
#include "Point.h"
#include "Color.h"

class AreaLight : public Light {
 public:
  AreaLight(const Point& position, const Color& color, const double& r);
  virtual ~AreaLight();

  virtual void preprocess();
  virtual double getLight(Color& light_color, Vector& light_direction,
                          const RenderContext& context, const Point& pos) const;

 private:
  AreaLight(const AreaLight&);
  AreaLight& operator=(const AreaLight&);

  Point position;
  Color color;
  double r = 1.0;
};

#endif

