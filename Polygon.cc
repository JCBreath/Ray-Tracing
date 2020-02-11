
#include "Polygon.h"
#include "BoundingBox.h"
#include "HitRecord.h"
#include "Point.h"
#include "Ray.h"
#include "Vector.h"
#include <math.h>

Polygon::Polygon(Material* material, const Point& p0, const Point& p1, const Point& p2)
  : Primitive(material), p0(p0), p1(p1), p2(p2)
{
  Vector v01(p1 - p0);
  Vector v02(p2 - p0);
  n = Cross(v01, v02);
  n.normalize();
  d = Dot(n, p0);
}

Polygon::~Polygon()
{
}

void Polygon::getBounds(BoundingBox& bbox) const
{
  bbox.extend(p0);
  bbox.extend(p1);
  bbox.extend(p2);
}

void Polygon::intersect(HitRecord& hit, const RenderContext&, const Ray& ray, double time) const
{
  double denom = Dot(n, ray.direction());
  
  double denom_o02 = Dot(n, ray.direction());
  double denom_o12 = Dot(n, ray.direction());
  if(Abs(denom) > 1.e-6){
    double t = (d-Dot(n, ray.origin()))/denom;
    Point hit_point = ray.origin() + t * ray.direction();
    double denom_01_0h = Dot(Cross(Vector(p1 - p0), Vector(hit_point - p0)), n);
    double denom_12_1h = Dot(Cross(Vector(p2 - p1), Vector(hit_point - p1)), n);
    double denom_20_2h = Dot(Cross(Vector(p0 - p2), Vector(hit_point - p2)), n);
    if(denom_01_0h > 1.e-6 && denom_12_1h > 1.e-6 && denom_20_2h > 1.e-6)
      hit.hit(t, this, matl);
  }
}

void Polygon::normal(Vector& normal, const RenderContext&, const Point& hitpos,
                    const Ray& ray, const HitRecord& hit) const
{
  normal = n;
}
