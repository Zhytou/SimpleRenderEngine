#ifndef SRE_TRIANGLE_HPP
#define SRE_TRIANGLE_HPP

#include <iostream>

#include "Material.hpp"
#include "Ray.hpp"
#include "Vec.hpp"

struct HitResult {
  int triangleId;
  bool isHit;
  double distance;
  Vec3<float> hitPoint;
  Vec3<float> normal;
  Material material;

  HitResult() : isHit(false), distance(0) {}
  HitResult(const bool& _hit, const double& _dis, const Vec3<float>& _point,
            const Vec3<float>& _n, const Material& _m)
      : isHit(_hit),
        distance(_dis),
        hitPoint(_point),
        normal(_n),
        material(_m) {}
};

// 三角形
class Triangle {
 private:
  Vec3<float> p1, p2, p3;  // 三顶点
  Vec3<float> normal;      // 法向量
  Material material;       // 材质

 public:
  Triangle(const Vec3<float>& _p1, const Vec3<float>& _p2,
           const Vec3<float>& _p3, const Material& _m)
      : p1(_p1),
        p2(_p2),
        p3(_p3),
        normal(normalize(cross(p2 - p1, p3 - p1))),
        material(_m) {}
  Triangle(const Vec3<float>& _p1, const Vec3<float>& _p2,
           const Vec3<float>& _p3, const Vec3<float>& _n, const Material& _m)
      : p1(_p1), p2(_p2), p3(_p3), normal(normalize(_n)), material(_m) {}
  ~Triangle() {}

  // 与光线求交
  void hit(const Ray& ray, HitResult& res) const {
    Vec3<float> origin = ray.getOrigin();
    Vec3<float> direction = ray.getDirection();
    Vec3<float> normal = this->normal;

    if (dot(normal, direction) > 0) {
      // 入射光线与法线必须呈钝角，但后续判断交点是否在三角形内部，需要使用原生法向量
      normal = -normal;
    }

    if (dot(normal, direction) == 0) {
      res.isHit = false;
      res.normal = normal;
      return;
    }

    float t = (dot(normal, p1) - dot(normal, origin)) / dot(normal, direction);
    if (t < 0.001) {
      res.isHit = false;
      res.normal = normal;
      res.distance = t;
      return;
    }

    Vec3<float> p = ray.pointAt(t);
    Vec3<float> c1 = cross(p2 - p1, p - p1);
    Vec3<float> c2 = cross(p3 - p2, p - p2);
    Vec3<float> c3 = cross(p1 - p3, p - p3);
    if (dot(c1, this->normal) < 0 || dot(c2, this->normal) < 0 ||
        dot(c3, this->normal) < 0) {
      res.isHit = false;
      res.normal = normal;
      res.distance = t;
      res.hitPoint = p;
      return;
    }

    res.isHit = true;
    res.hitPoint = p;
    res.normal = normal;
    res.distance = t;
    res.material = material;
    return;
  };

  // 与光线求交 只不过在之前res的基础上比较
  void hitCompare(const Ray& ray, HitResult& res,
                  const float& curDistance) const {
    Vec3<float> origin = ray.getOrigin();
    Vec3<float> direction = ray.getDirection();
    Vec3<float> normal = this->normal;

    if (dot(normal, direction) > 0) {
      // 入射光线与法线必须呈钝角，但后续判断交点是否在三角形内部，需要使用原生法向量
      normal = -normal;
    }

    if (dot(normal, direction) == 0) {
      res.isHit = false;
      return;
    }

    float t = (dot(normal, p1) - dot(normal, origin)) / dot(normal, direction);
    if (t < 0.001 || t >= curDistance) {
      res.isHit = false;
      return;
    }

    Vec3<float> p = ray.pointAt(t);
    Vec3<float> c1 = cross(p2 - p1, p - p1);
    Vec3<float> c2 = cross(p3 - p2, p - p2);
    Vec3<float> c3 = cross(p1 - p3, p - p3);
    if (dot(c1, this->normal) < 0 || dot(c2, this->normal) < 0 ||
        dot(c3, this->normal) < 0) {
      res.isHit = false;
      return;
    }

    res.isHit = true;
    res.hitPoint = p;
    res.normal = normal;
    res.distance = t;
    res.material = material;
    return;
  };

  // getter
  Vec3<float> getRandomPoint() const {
    Vec3<float> e1 = p2 - p1, e2 = p3 - p2;
    float a = sqrt(randFloat(1)), b = randFloat(1);
    return e1 * a + e2 * a * b + p1;
  }
  Material getMaterial() const { return material; }

  void printStatus() const {
    std::cout << "vertex 1: " << p1.x << '\t' << p1.y << '\t' << p1.z << '\n'
              << "vertex 2: " << p2.x << '\t' << p2.y << '\t' << p2.z << '\n'
              << "vertex 3: " << p3.x << '\t' << p3.y << '\t' << p3.z << '\n'
              << "normal: " << normal.x << '\t' << normal.y << '\t' << normal.z
              << '\n';
    material.printStatus();
    std::cout << std::endl;
  }
};

#endif