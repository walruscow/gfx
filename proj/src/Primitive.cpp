#include "Primitive.hpp"

#include <algorithm>

#include "polyroots.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"

namespace {

bool isValid(double t) {
  return t > 0 && !isZero(t);
}

double mostValid(double t1, double t2) {
  if (!isValid(t1)) {
    return t2;
  }
  if (!isValid(t2)) {
    return t1;
  }
  return std::min(t1, t2);
}

} // Anonymous


bool Cube::intersects(
    const Ray& ray, HitRecord* hitRecord, const Matrix4x4& inverseTransform) {
  // New ray
  const auto a = inverseTransform * ray.start;
  const auto b = inverseTransform * ray.other;
  const auto dir = b - a;

  // Ohkay
  double t = solveIntersection(a, dir);
  if (t < 0) return false;

  auto localPoint = a + t * dir;

  // We will be on some face, and that coordinate will be ~ 0 or 1
  Vector3D localNorm(0, 0, 0);
  auto getNormCoord = [&] (int c) {
    if (isEqual(localPoint[c], 1.0)) {
      localNorm[c] = 1;
    }
    else if (isZero(localPoint[c])) {
      localNorm[c] = -1;
    }
  };
  for (int c = 0; c < 3; ++c) getNormCoord(c);

  // t is unchanged by this
  // Remember to use the *original* intersection point
  auto point = ray.at(t);

  // To get the appropriate normal vector, we must also transpose
  // the inverse transform
  auto norm = inverseTransform.transpose() * localNorm;
  norm.normalize();

  // We will return whether or not this intersection was
  // better than whatever was already stored there
  return hitRecord->update(norm, point, t);
}

double Cube::solveIntersection(const Point3D& a, const Vector3D& dir) {
  int count = 0;
  double minT = -1;
  for (const auto& face : {f1, f2, f3, f4, f5, f6}) {
    // Check intersection with each face
    // We will use a parametric eqn for a square
    // p0 + (p1 - p0)r + (p3 - p0)s = p
    // 0 <= r <= 1; 0 <= s <= 1
    const auto& p0 = face[0];
    const auto& p1 = face[1];
    const auto& p3 = face[3];

    Vector3D col1 = p1 - p0;
    Vector3D col2 = p3 - p0;
    Vector3D col3 = -dir;
    Vector3D rhs = -p0 + Vector3D(a[0], a[1], a[2]);

    double detA = det(col1, col2, col3);
    if (isZero(detA)) {
      continue;
    }
    double c1 = det(rhs, col2, col3);
    double r = c1 / detA;
    if (r < 0 || r > 1) {
      // Outside of square
      continue;
    }

    double c2 = det(col1, rhs, col3);
    double s = c2 / detA;
    if (s < 0 || s > 1) {
      // Outside of square
      continue;
    }

    double c3 = det(col1, col2, rhs);
    double t = c3 / detA;
    if (t < 0 || isZero(t)) {
      continue;
    }

    // Intersection on this face
    if (count == 0) {
      count = 1;
      minT = t;
    }
    else if (count == 1) {
      // If we have already found an intersection, then we just found the
      // second intersection. There cannot be more than 2, so we can stop.
      count = 2;
      // If the new one is zero or the new one is bigger, do not keep it
      minT = (isZero(t) || minT <= t) ? minT : t;
      break;
    }
  }

  // And discard near zero
  return isZero(minT) ? -1 : minT;
}

bool Sphere::intersects(
    const Ray& ray, HitRecord* hitRecord, const Matrix4x4& inverseTransform) {
  // New ray
  const auto p1 = inverseTransform * ray.start;
  const auto p2 = inverseTransform * ray.other;
  const auto dir = p2 - p1;

  double t = solveIntersection(p1, dir);
  if (t < 0) return false;

  // Since this is a unit sphere, the norm at p is a vector to p
  auto localPoint = p1 + t * dir;
  Vector3D localNorm(localPoint[0], localPoint[1], localPoint[2]);

  // t is unchanged by this
  // Remember to use the *original* intersection point
  auto point = ray.at(t);

  // To get the appropriate normal vector, we must also transpose
  // the inverse transform
  auto norm = inverseTransform.transpose() * localNorm;
  norm.normalize();

  // We will return whether or not this intersection was
  // better than whatever was already stored there
  return hitRecord->update(norm, point, t);
}

double Sphere::solveIntersection(const Point3D& p1, const Vector3D& dir) {
  // Now do intersection against unit sphere centred at origin
  // x^2 + y^2 + z^2 = 1
  // We want to check whether or not p.dot(p) == 1 for some p on the line
  // pt on line is p = p1 + t * dir
  // Expand into  (dir^2)*(t^2) + (dir*p1)*2t - 1 = 0
  const auto a = dir.dot(dir);
  // No dot product is defined for points and vectors
  const auto b = 2 * (dir[0]*p1[0] + dir[1]*p1[1] + dir[2]*p1[2]);
  //... or points and points
  const auto c = p1[0]*p1[0] + p1[1]*p1[1] + p1[2]*p1[2] -1;
  double roots[2];

  const auto numRoots = quadraticRoots(a, b, c, roots);

  if (numRoots == 0) {
    // No intersection
    return -1;
  }

  // Only t >= 0 matters (and we should probably discard < EPSILON as well)
  double t = roots[0];
  if (numRoots == 2 && roots[1] > 0 && !isZero(roots[1])) {
    // Take smallest non-negative value
    t = (t < 0) ? roots[1] : std::min(t, roots[1]);
  }
  if (t < 0 || isZero(t)) {
    // No *valid* intersections
    return -1;
  }
  return t;
}

bool Cylinder::intersects(const Ray& ray,
                          HitRecord* hitRecord,
                          const Matrix4x4& inverseTransform) {
  // New ray
  const auto p1 = inverseTransform * ray.start;
  const auto p2 = inverseTransform * ray.other;
  const auto dir = p2 - p1;

  double t = solveIntersection(p1, dir);
  if (t < 0) return false;

  auto localPoint = p1 + t * dir;
  Vector3D localNorm(0, 0, 0);
  if (isZero(localPoint[2] + 1)) {
    localNorm = Vector3D(0, 0, -1);
  }
  else if (isZero(localPoint[2] - 1)) {
    localNorm = Vector3D(0, 0, 1);
  }
  else {
    // Not on one of the tops: Just like a circle
    localNorm = Vector3D(localPoint[0], localPoint[1], 0);
  }

  // t is unchanged by this
  // Remember to use the *original* intersection point
  auto point = ray.at(t);

  // To get the appropriate normal vector, we must also transpose
  // the inverse transform
  auto norm = inverseTransform.transpose() * localNorm;
  norm.normalize();

  // We will return whether or not this intersection was
  // better than whatever was already stored there
  return hitRecord->update(norm, point, t);
}

double Cylinder::solveIntersection(const Point3D& p1, const Vector3D& dir) {
  auto t = mostValid(getCapT(p1, dir), getWallT(p1, dir));
  return isValid(t) ? t : -1;
}

double Cylinder::getCapT(const Point3D& p1, const Vector3D& dir) {
  // Get the best t for the intersection with the cylinder caps
  double cap1 = -1;
  double cap2 = -1;
  if (!isZero(dir[2])) {
    cap1 = (1 - p1[2]) / dir[2];
    cap2 = (-1 - p1[2]) / dir[2];
  }

  auto inCircle = [&] (double t) -> bool {
    const auto p = (p1 + t * dir);
    return (p[0] * p[0] + p[1] * p[1]) <= 1;
  };

  // Take care of special circle case
  if (!inCircle(cap1)) {
    cap1 = -1;
  }
  if (!inCircle(cap2)) {
    cap2 = -1;
  }

  return mostValid(cap1, cap2);
}

double Cylinder::getWallT(const Point3D& p1, const Vector3D& dir) {
  // Get the best t for the intersection with cylinder walls
  double a = dir[0] * dir[0] + dir[1] * dir[1];
  double b = 2 * (p1[0] * dir[0] + p1[1] * dir[1]);
  double c = p1[0] * p1[0] + p1[1] * p1[1] - 1;

  double roots[2];
  const auto numRoots = quadraticRoots(a, b, c, roots);

  if (numRoots == 0) {
    return -1;
  }

  const auto inCap = [&] (double t) -> bool {
    const auto z = (p1 + t * dir)[2];
    return z <= 1 && z >= -1;
  };

  if (numRoots == 1) {
    // Return root if valid else -1
    return (isValid(roots[0]) && inCap(roots[0])) ? roots[0] : -1;
  }

  // Set to invalid if not within the caps
  for (auto i = 0; i < 2; ++i) {
    if (!inCap(roots[i])) {
      roots[i] = -1;
    }
  }

  return mostValid(roots[0], roots[1]);
}
