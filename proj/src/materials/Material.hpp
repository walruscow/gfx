#pragma once

#include "algebra.hpp"

class HitRecord;
class Light;

// It's a phong material kind of world
class Material {
 public:
  Material(const Colour& ks, double shininess);
  virtual ~Material() = default;

  Colour getColour(const HitRecord& hitRecord) const;

  Colour lightColour(const Colour& colour, // Colour at point
                     const Vector3D& dir, // View direction
                     const Light& light, // Light to light with
                     const HitRecord& hitRecord) const;

 protected:
  virtual Colour getKd(const HitRecord& hitRecord) const = 0;

 private:
  Colour ks;
  double shininess;
};