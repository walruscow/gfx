#pragma once

class Primitive {
 public:
  virtual ~Primitive();
  virtual void walk_gl(bool picking) const = 0;
};

class Sphere : public Primitive {
 public:
  virtual ~Sphere();
  virtual void walk_gl(bool picking) const;
};
