
#ifndef Polygon_h
#define Polygon_h

#include "Primitive.h"
#include "Point.h"
class Ray;

class Polygon : public Primitive {
 public:
  Polygon(Material* material, const Point& p0, const Point& p1, const Point& p2);
  virtual ~Polygon();

  virtual void getBounds(BoundingBox& bbox) const;
  virtual void intersect(HitRecord& hit, const RenderContext& context, const Ray& ray, double time) const;
  virtual void normal(Vector& normal, const RenderContext& context,
                      const Point& hitpos, const Ray& ray, const HitRecord& hit) const;
 protected:
  Point p0, p1, p2;
  Vector n;
  double d;
};

#endif
