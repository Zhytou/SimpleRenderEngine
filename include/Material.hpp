#ifndef SRE_MATERIAL_HPP
#define SRE_MATERIAL_HPP

#include <iostream>
#include <fstream>
#include <string>

#include "Vec.hpp"

namespace sre {

class Material {
 private:
  std::string name;            // name 材料名称
  Vec3<float> emission;        // Ke 自射光 对应xml文件中radiance
  Vec3<float> ambience;        // Ka 环境光
  Vec3<float> diffusion;       // Kd 漫射光
  Vec3<float> specularity;     // Ks 镜面光
  Vec3<float> transmittance;   // Tr 透射光
  float shiness;               // Ns 反光强度
  float refraction;            // Ni 折射率
  bool emisssive;              // 是否发光
  char* ambientTexture;    // map_Ka 纹理
  char* diffuseTexture;   // map_Kd 纹理
  char* specularTexture;   // map_Ks 纹理

 public:
  Material() = default;
  ~Material() = default;

  // getter.
  bool isEmissive() const { return emisssive; }
  bool isDiffusive() const {
    return diffusion.x != 0 || diffusion.y != 0 || diffusion.z != 0;
  }
  bool isSpecular() const {
    return specularity.x != 0 || specularity.y != 0 || specularity.z != 0;
  }
  bool isTransmissive() const { return refraction >= 1; }

  std::string getName() const { return name; }
  Vec3<float> getEmission() const { return emission; }
  Vec3<float> getAambience() const { return ambience; }
  Vec3<float> getDiffusion() const { return diffusion; }
  Vec3<float> getSpecularity() const { return specularity; }
  Vec3<float> getTransmittance() const { return transmittance; }
  float getShiness() const { return shiness; }
  float getRefraction() const { return refraction; }

  // print.
  void printStatus() const {
    std::cout << "material" << '\n'
              << "diffuse: " << diffusion.x << '\t' << diffusion.y << '\t'
              << diffusion.z << '\n'
              << "specular: " << specularity.x << '\t' << specularity.y << '\t'
              << specularity.z << '\n';
  }

  // setter.
  void setName(const std::string& n) { name = n; }
  void setEmissive(bool e) { emisssive = e; }
  void setEmission(float x, float y, float z) {
    emission.x = x;
    emission.y = y;
    emission.z = z;
  }
  void setAmbience(float x, float y, float z) {
    ambience.x = x;
    ambience.y = y;
    ambience.z = z;
  }
  void setDiffusion(float x, float y, float z) {
    diffusion.x = x;
    diffusion.y = y;
    diffusion.z = z;
  }
  void setSpecularity(float x, float y, float z) {
    specularity.x = x;
    specularity.y = y;
    specularity.z = z;
  }
  void setTransmittance(float x, float y, float z) {
    transmittance.x = x;
    transmittance.y = y;
    transmittance.z = z;
  }
  void setShiness(float s) { shiness = s; }
  void setRefraction(float r) { refraction = r; }
  void setAmbientTexture(const std::string& at) { 
    std::ifstream ifs(at, std::ifstream::in);
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    ambientTexture = new char[length];
    ifs.read(ambientTexture, length);
  }  
  void setDiffuseTexture(const std::string& dt) {
    std::ifstream ifs(dt, std::ifstream::in);
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    diffuseTexture = new char[length];
    ifs.read(diffuseTexture, length);
  }
  void setSpecularTexture(const std::string& st) {
    std::ifstream ifs(st, std::ifstream::in);
    ifs.seekg(0, ifs.end);
    int length = ifs.tellg();
    ifs.seekg(0, ifs.beg);
    specularTexture = new char[length];
    ifs.read(specularTexture, length);
  }
};
}  // namespace sre

#endif
