#pragma once

#include "Cube.hpp"
#include "Primitive.hpp"

// A cylinder goes from z = -1 to 1, has radius 1 on xy
class Cylinder : public Primitive {
 public:
  bool intersects(const Ray& ray,
                  HitRecord* hitRecord,
                  const Matrix4x4& inverseTransform) const override;
  Point3D getMinPoint(const Matrix4x4& inverseTransform) const override;
  Point3D getMaxPoint(const Matrix4x4& inverseTransform) const override;

  std::vector<Point3D> getBoundingBox(const Matrix4x4& inverseTransform)
      const override;

 private:
  double solveIntersection(const Point3D& p1, const Vector3D& dir) const;
  double getCapT(const Point3D& p1, const Vector3D& dir) const;
  double getWallT(const Point3D& p1, const Vector3D& dir) const;

  Cube boundingCube;
};
