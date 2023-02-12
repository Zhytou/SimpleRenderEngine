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

#include "BVH.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Ray.hpp"
#include "Triangle.hpp"
#include "Vec.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include "../third-parties/tinyobjloader/tiny_obj_loader.h"

namespace sre {
class Tracer {
 private:
  BVH *scenes;
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
  Vec3<float> trace(const Ray &ray, size_t depth);
  void printStatus();

 public:
  Tracer(size_t _depth = 3, size_t _samples = 3, float _p = 0.5)
      : scenes(nullptr), maxDepth(_depth), samples(_samples), thresholdP(_p) {}
  ~Tracer() {
    if (scenes != nullptr) {
      delete scenes;
    }
  }

  void loadExampleScene();
  void load(const std::string &pathName, const std::string &fileName);
  cv::Mat render();
};
}  // namespace sre

#endif