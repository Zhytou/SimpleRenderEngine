#ifndef SRE_TRACE_HPP
#define SRE_TRACE_HPP

#include <omp.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "Camera.hpp"
#include "Light.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"
#include "Vec.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../third-parties/tinyobjloader/tiny_obj_loader.h"

class Tracer {
 private:
  std::vector<Triangle> triangles;
  Camera camera;
  Light light;
  size_t maxDepth;
  size_t samples;
  float thresholdP;

 private:
  bool loadConfiguration(
      const std::string &configName,
      std::unordered_map<std::string, Vec3<float>> &lightRadiances);
  bool loadModel(
      const std::string &modelName, const std::string &pathName,
      const std::unordered_map<std::string, Vec3<float>> &lightRadiances);
  Vec3<float> shade(const int &row, const int &col);
  Vec3<float> trace(const Ray &ray, size_t depth);
  void shoot(const Ray &ray, HitResult &res);
  void printStatus();

 public:
  Tracer(size_t _depth = 3, size_t _samples = 5, float _p = 0.5)
      : maxDepth(_depth), samples(_samples), thresholdP(_p) {}

  void loadExampleScene();
  void load(const std::string &pathName, const std::string &fileName);
  cv::Mat render();
};

void Tracer::loadExampleScene() {
  float eyePosZ = 2;
  // Configuration
  camera.setWidth(500);
  camera.setHeight(500);
  camera.setFovy(90);
  camera.setPosition(0, 0, -eyePosZ);
  camera.setTarget(0, 0, -1);
  camera.setWorld(0, 1, 0);

  // Scene-Box-h,w,l
  float w = eyePosZ;
  float h = eyePosZ;
  float l = eyePosZ * 2;

  // Scene-Light
  Material m;
  m.setEmissive(true);
  m.setEmission(34, 24, 8);
  m.setDiffusion(0, 0, 0);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-0.5, 2, 2), Vec3<float>(-0.5, 2, 0.5),
                         Vec3<float>(0.5, 2, 0.5), m);
  triangles.emplace_back(Vec3<float>(-0.5, 2, 2), Vec3<float>(0.5, 2, 2),
                         Vec3<float>(0.5, 2, 0.5), m);
  triangles.emplace_back(Vec3<float>(-0.5, 2, -2), Vec3<float>(-0.5, 2, -0.5),
                         Vec3<float>(0.5, 2, -0.5), m);
  triangles.emplace_back(Vec3<float>(-0.5, 2, -2), Vec3<float>(0.5, 2, -2),
                         Vec3<float>(0.5, 2, -0.5), m);
  light.setLight(0, 0.75, m.getEmission());
  light.setLight(1, 0.75, m.getEmission());
  light.setLight(2, 0.75, m.getEmission());
  light.setLight(3, 0.75, m.getEmission());

  // Scene-Ground
  m.setEmissive(false);
  m.setEmission(0, 0, 0);
  m.setDiffusion(0.79, 0.76, 0.73);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-w * 3, -h, l),
                         Vec3<float>(-w * 3, -h, -1),
                         Vec3<float>(w * 3, -h, -1), m);
  triangles.emplace_back(Vec3<float>(-w * 3, -h, l), Vec3<float>(w * 3, -h, l),
                         Vec3<float>(w * 3, -h, -1), m);

  // Scene-Top
  m.setEmissive(false);
  m.setDiffusion(0.79, 0.76, 0.73);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-w, h, l), Vec3<float>(-w, h, -1),
                         Vec3<float>(w, h, -1), m);
  triangles.emplace_back(Vec3<float>(-w, h, l), Vec3<float>(w, h, l),
                         Vec3<float>(w, h, -1), m);

  // Scene-BackWall
  m.setEmissive(false);
  m.setDiffusion(0.79, 0.76, 0.73);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-w, -h, l), Vec3<float>(-w, h, l),
                         Vec3<float>(w, h, l), m);
  triangles.emplace_back(Vec3<float>(-w, -h, l), Vec3<float>(w, -h, l),
                         Vec3<float>(w, h, l), m);

  // Scene-RightWall
  m.setEmissive(false);
  m.setDiffusion(0.2, 0.76, 0);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-w, h, 0), Vec3<float>(-w, -h, 0),
                         Vec3<float>(-w, h, l), m);
  triangles.emplace_back(Vec3<float>(-w, h, l), Vec3<float>(-w, -h, l),
                         Vec3<float>(-w, -h, 0), m);

  // Scene-LeftWall
  m.setEmissive(false);
  m.setDiffusion(0, 0.24, 0.9);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(w, h, 0), Vec3<float>(w, -h, 0),
                         Vec3<float>(w, h, l), m);
  triangles.emplace_back(Vec3<float>(w, h, l), Vec3<float>(w, -h, l),
                         Vec3<float>(w, -h, 0), m);

  // Scene-Shape
  m.setEmissive(false);
  m.setDiffusion(0, 0.9, 0);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(-1.2, 1, 1), Vec3<float>(-0.25, -1.5, 1),
                         Vec3<float>(-1.5, -1.5, 3), m);
  m.setEmissive(false);
  m.setDiffusion(0, 0, 0.8);
  m.setSpecularity(0, 0, 0);
  m.setTransmittance(1, 1, 1);
  m.setShiness(1);
  m.setRefraction(0);
  triangles.emplace_back(Vec3<float>(1.2, 1, 1), Vec3<float>(0.25, -1.5, 1),
                         Vec3<float>(1.5, -1.5, 3), m);

  printStatus();
}

bool Tracer::loadConfiguration(
    const std::string &configName,
    std::unordered_map<std::string, Vec3<float>> &lightRadiances) {
  std::ifstream ifs;
  ifs.open(configName, std::ios::in);
  if (!ifs.is_open()) {
    return false;
  }

  std::string buf;
  int i, j;

  int width, height;
  float fovy;
  float xyzs[3][3];

  // xml
  getline(ifs, buf);
  // camera
  getline(ifs, buf);
  i = 0;
  while (i < buf.size()) {
    switch (buf[i]) {
      case 'w':
        assert(buf[i + 1] == 'i' && buf[i + 2] == 'd' && buf[i + 3] == 't' &&
               buf[i + 4] == 'h' && buf[i + 5] == '=' && buf[i + 6] == '\"');
        i += 7;
        j = buf.find('\"', i);
        width = stoi(buf.substr(i, j - i));
        i = j + 1;
        break;
      case 'h':
        assert(buf[i + 1] == 'e' && buf[i + 2] == 'i' && buf[i + 3] == 'g' &&
               buf[i + 4] == 'h' && buf[i + 5] == 't' && buf[i + 6] == '=' &&
               buf[i + 7] == '\"');
        i += 8;
        j = buf.find('\"', i);
        height = stoi(buf.substr(i, j - i));
        i = j + 1;
        break;
      case 'f':
        assert(buf[i + 1] == 'o' && buf[i + 2] == 'v' && buf[i + 3] == 'y' &&
               buf[i + 4] == '=' && buf[i + 5] == '\"');
        i += 6;
        j = buf.find('\"', i);
        fovy = stof(buf.substr(i, j - i));
        i = j + 1;
        break;
      default:
        i += 1;
        break;
    }
  }

  // eye-lookat-up
  for (int row = 0; row < 3; row++) {
    getline(ifs, buf);
    i = 0;

    int col = 0;
    while (i < buf.size()) {
      if (i > 0 && buf[i - 1] == ' ' &&
          (buf[i] == 'x' || buf[i] == 'y' || buf[i] == 'z')) {
        assert(buf[i + 1] == '=' && buf[i + 2] == '\"');
        i += 3;
        j = buf.find('\"', i);
        xyzs[row][col] = stof(buf.substr(i, j - i));
        i = j + 1;
        col += 1;
      } else {
        i += 1;
      }
    }
  }

  // light radiance
  while (getline(ifs, buf)) {
    std::string mtlname;
    Vec3<float> radiance;
    i = 0;
    while (i < buf.size()) {
      if (i > 0 && buf[i - 1] == ' ' && buf[i] == 'm') {
        assert(buf.substr(i, 9) == "mtlname=\"");
        i += 9;
        j = buf.find('\"', i);
        mtlname = buf.substr(i, j - i);
        i = j + 1;
      } else if (i > 0 && buf[i - 1] == ' ' && buf[i] == 'r') {
        assert(buf.substr(i, 10) == "radiance=\"");
        i += 10;
        j = buf.find(',', i);
        radiance.x = stof(buf.substr(i, j - i));
        i = j + 1;

        j = buf.find(',', i);
        radiance.y = stof(buf.substr(i, j - i));
        i = j + 1;

        j = buf.find('\"', i);
        radiance.z = stof(buf.substr(i, j - i));
        i = j + 1;
      } else {
        i += 1;
      }
    }
    if (!mtlname.empty()) {
      lightRadiances.emplace(mtlname, radiance);
    }
  }

  camera.setWidth(width);
  camera.setHeight(height);
  camera.setFovy(fovy);
  camera.setPosition(xyzs[0][0], xyzs[0][1], xyzs[0][2]);
  camera.setTarget(xyzs[1][0], xyzs[1][1], xyzs[1][2]);
  camera.setWorld(xyzs[2][0], xyzs[2][1], xyzs[2][2]);
  return true;
}

bool Tracer::loadModel(
    const std::string &modelName, const std::string &pathName,
    const std::unordered_map<std::string, Vec3<float>> &lightRadiances) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string warn;
  std::string err;
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, &warn,
                        modelName.c_str(), pathName.c_str())) {
    return false;
  }

  std::vector<Material> actualMaterials;
  for (const auto &material : materials) {
    Material actualMaterial;

    auto itr = lightRadiances.find(material.name);
    if (itr != lightRadiances.end()) {
      actualMaterial.setEmissive(true);
      actualMaterial.setEmission(itr->second.x, itr->second.y, itr->second.z);
    }
    actualMaterial.setDiffusion(material.diffuse[0], material.diffuse[1],
                                material.diffuse[2]);
    actualMaterial.setSpecularity(material.specular[0], material.specular[1],
                                  material.specular[2]);
    actualMaterial.setTransmittance(material.transmittance[0],
                                    material.transmittance[1],
                                    material.transmittance[2]);
    actualMaterial.setShiness(material.shininess);
    actualMaterial.setRefraction(material.ior);
    actualMaterials.emplace_back(actualMaterial);
  }

  for (const auto &shape : shapes) {
    assert(shape.mesh.material_ids.size() ==
           shape.mesh.num_face_vertices.size());

    size_t triagnleNum = shape.mesh.material_ids.size();
    for (size_t face_i = 0; face_i < triagnleNum; face_i++) {
      Vec3<float> points[3];
      for (size_t point_i = 0; point_i < 3; point_i++) {
        int vertex_index =
            shape.mesh.indices[face_i * 3 + point_i].vertex_index;

        points[point_i].x = attrib.vertices[vertex_index * 3 + 0];
        points[point_i].y = attrib.vertices[vertex_index * 3 + 1];
        points[point_i].z = attrib.vertices[vertex_index * 3 + 2];
      }

      Vec3<float> point_normals[3];
      for (size_t point_i = 0; point_i < 3; point_i++) {
        int normal_index =
            shape.mesh.indices[face_i * 3 + point_i].normal_index;

        point_normals[point_i].x = attrib.normals[normal_index * 3 + 0];
        point_normals[point_i].y = attrib.normals[normal_index * 3 + 1];
        point_normals[point_i].z = attrib.normals[normal_index * 3 + 2];
      }

      Vec3<float> normal = cross(points[1] - points[0], points[2] - points[0]);
      if (dot(point_normals[0], normal) < 0) {
        normal = -normal;
      }

      Material material = actualMaterials[shape.mesh.material_ids[face_i]];
      if (material.isEmissive()) {
        int id = triangles.size();
        float area =
            cross(points[1] - points[0], points[2] - points[0]).length() / 2;
        light.setLight(id, area, material.getEmission());
      }
      triangles.emplace_back(points[0], points[1], points[2], normal, material);
    }
  }
  return true;
}

void Tracer::load(const std::string &pathName, const std::string &fileName) {
  // Configuration -Camera
  std::unordered_map<std::string, Vec3<float>> lightRadiances;
  std::string configName = pathName + fileName + ".xml";
  if (!loadConfiguration(configName, lightRadiances)) {
    std::cout << "Camera config loading fails!" << std::endl;
    return;
  }

  // Scene
  std::string modelName = pathName + fileName + ".obj";
  if (!loadModel(modelName, pathName, lightRadiances)) {
    std::cout << "Model loading fails!" << std::endl;
    return;
  }
  printStatus();
}

cv::Mat Tracer::render() {
  int height = camera.getHeight(), width = camera.getWidth();
  // 注意：CV_32F白色为（1，1，1）对应CV_8U的白色（255，255，255）
  cv::Mat img(cv::Size(width, height), CV_32FC3, cv::Scalar(0, 0, 0));

#pragma omp parallel for num_threads(100)
  for (int row = 0; row < height; row++) {
#pragma omp parallel for num_threads(100)
    for (int col = 0; col < width; col++) {
      Vec3<float> color = shade(row, col);

      img.at<cv::Vec3f>(row, col)[2] = color.x;
      img.at<cv::Vec3f>(row, col)[1] = color.y;
      img.at<cv::Vec3f>(row, col)[0] = color.z;
    }
  }

  return img;
}

Vec3<float> Tracer::shade(const int &row, const int &col) {
  Vec3<float> color(0, 0, 0);
#pragma omp parallel for num_threads(5)
  for (int k = 0; k < samples; k++) {
    Ray ray = camera.getRay(row, col);
    color += trace(ray, 0);
  }

  color /= samples;
  return color;
}

void Tracer::shoot(const Ray &ray, HitResult &res) {
  HitResult curRes;
#pragma omp parallel for num_threads(100)
  for (size_t i = 0; i < triangles.size(); i++) {
    curRes.triangleId = i;
    if (!res.isHit) {
      triangles[i].hit(ray, curRes);
      if (curRes.isHit) {
        res = curRes;
      }
    } else {
      triangles[i].hitCompare(ray, curRes, res.distance);
      if (curRes.isHit && res.distance > curRes.distance) {
        res = curRes;
      }
    }
  }
  return;
}

Vec3<float> Tracer::trace(const Ray &ray, size_t depth) {
  if (depth >= maxDepth) {
    return Vec3<float>(0, 0, 0);
  }

  float possibility = randFloat(1);
  if (possibility > thresholdP) {
    return Vec3<float>(0, 0, 0);
  }

  HitResult res;
  shoot(ray, res);
  if (!res.isHit) {
    return Vec3<float>(0, 0, 0);
  }

  float cosine = fabs(dot(ray.getDirection(), res.normal));
  float dis = depth == 0 ? 1 : distance(res.hitPoint, ray.getOrigin());
  Vec3<float> directLight(0, 0, 0), indirectLight(0, 0, 0);

  // return Vec3<float>(1, 1, 1) * res.material.getDiffusion();

  if (res.material.isEmissive()) {
    // 直接光照 ——发光物
    directLight = res.material.getEmission() * cosine / (dis * dis);
  } else {
    // 直接光照 ——节省路径（自己打过去）
    int lightId = randInt(light.getLightSize());
    int triangleId = light.getRandomTriangleId(lightId);
    static float pdfLight = 1 / light.getLightAreaAt(lightId);
    Vec3<float> lightPoint = triangles[triangleId].getRandomPoint();
    assert(triangles[triangleId].getMaterial().isEmissive());
    Vec3<float> radiance = triangles[triangleId].getMaterial().getEmission();
    dis = distance(res.hitPoint, lightPoint);

    // 检查是否有障碍
    Ray tmpRay(res.hitPoint, lightPoint - res.hitPoint);
    HitResult tmpRes;
    shoot(tmpRay, tmpRes);
    if (tmpRes.isHit && tmpRes.triangleId == triangleId) {
      directLight += radiance * res.material.getDiffusion() * cosine /
                     (dis * dis * pdfLight);
    }

    static float pdf = 1 / (2 * PI);
    dis = depth == 0 ? 1 : distance(res.hitPoint, ray.getOrigin());

    // 间接光照
    if (res.material.isDiffusive()) {
      // 漫反射
      Ray reflectRay =
          randomReflectRay(res.hitPoint, ray.getDirection(), res.normal);
      Vec3<float> reflectLight = trace(reflectRay, depth + 1);
      indirectLight +=
          reflectLight * res.material.getDiffusion() * cosine / (dis * dis);
    }

    if (res.material.isSpecular()) {
      // 镜面反射
      Ray reflectRay =
          standardReflectRay(res.hitPoint, ray.getDirection(), res.normal);
      Vec3<float> reflectLight = trace(reflectRay, depth + 1);
      indirectLight += reflectLight * res.material.getSpecularity() *
                       pow(cosine, res.material.getShiness()) / (dis * dis);
    }

    if (res.material.isTransmissive()) {
      // 折射
      Ray refractRay =
          standardRefractRay(res.hitPoint, ray.getDirection(), res.normal,
                             res.material.getRefraction());
      Vec3<float> refractLight = trace(refractRay, depth + 1);
      indirectLight +=
          refractLight * res.material.getTransmittance() * cosine / (dis * dis);
    }

    indirectLight /= pdf;
  }

  return (directLight + indirectLight) / thresholdP;
}

void Tracer::printStatus() {
  // configuration
  std::cout << "sample number: " << samples << '\n'
            << "tracing depth: " << maxDepth << '\n';
  camera.printStatus();

  // shapes
  std::cout << "shapes" << '\n'
            << "triange number: " << triangles.size() << '\n';
  for (int i = 0; i < std::min(size_t(1), triangles.size()); i++) {
    std::cout << "No." << i << " triangle is following" << '\n';
    triangles[i].printStatus();
  }
  std::cout << std::endl;
}
#endif