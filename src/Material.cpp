#include <iostream>

#include "../include/Material.hpp"

namespace sre {

Material::Material() {}
Material::~Material() {}

std::unordered_map<std::string, Material*> Material::materials;

Material *Material::getInstance(const std::string& materialName) {
  if (materials.find(materialName) == materials.end()) {
    Material* nmat = new Material();
  }
  return materials[materialName];
}
void Material::releaseAllInstances() {
  for (auto& [name, mat] : materials) {
    delete mat;
    mat = nullptr;
  }
}

std::string Material::getName() const { return name; }
Vec3<float> Material::getEmission() const { return emission; }
Vec3<float> Material::getAmbience(const Vec2<float>& texCoord) const { 
  if (ambientTexture == nullptr) {
    return ambience; 
  } else {
    return ambientTexture->getColorAt(texCoord);
  }
}
Vec3<float> Material::getDiffusion(const Vec2<float>& texCoord) const {
  if (diffuseTexture == nullptr) {
    return diffusion; 
  } else {
    return diffuseTexture->getColorAt(texCoord);
  }
}
Vec3<float> Material::getSpecularity(const Vec2<float>& texCoord) const {
  if (specularTexture == nullptr) {
    return specularity; 
  } else {
    return specularTexture->getColorAt(texCoord);
  }
}
Vec3<float> Material::getTransmittance(const Vec2<float>& texCoord) const { return transmittance; }
float Material::getShiness() const { return shiness; }
float Material::getRefraction() const { return refraction; }

bool Material::isEmissive() const { return emisssive; }
bool Material::isDiffusive() const {
  return diffusion.x != 0 || diffusion.y != 0 || diffusion.z != 0;
}
bool Material::isSpecular() const {
  return specularity.x != 0 || specularity.y != 0 || specularity.z != 0;
}
bool Material::isTransmissive() const { return refraction >= 1; }

void Material::setName(const std::string& n) { name = n; }
void Material::setEmissive(bool e) { emisssive = e; }
void Material::setEmission(float x, float y, float z) {
  emission.x = x;
  emission.y = y;
  emission.z = z;
}
void Material::setAmbience(float x, float y, float z) {
  ambience.x = x;
  ambience.y = y;
  ambience.z = z;
}
void Material::setDiffusion(float x, float y, float z) {
  diffusion.x = x;
  diffusion.y = y;
  diffusion.z = z;
}
void Material::setSpecularity(float x, float y, float z) {
  specularity.x = x;
  specularity.y = y;
  specularity.z = z;
}
void Material::setTransmittance(float x, float y, float z) {
  transmittance.x = x;
  transmittance.y = y;
  transmittance.z = z;
}
void Material::setShiness(float s) { shiness = s; }
void Material::setRefraction(float r) { refraction = r; }
void Material::setAmbientTexture(const std::string& at) {
  ambientTexture = Texture::getInstance(at);
}
void Material::setDiffuseTexture(const std::string& dt) {
  diffuseTexture = Texture::getInstance(dt);
}
void Material::setSpecularTexture(const std::string& st) {
  specularTexture = Texture::getInstance(st);
}

// print.
void Material::printStatus() const {
  std::cout << "material" << '\n'
            << "diffuse: " << diffusion.x << '\t' << diffusion.y << '\t'
            << diffusion.z << '\n'
            << "specular: " << specularity.x << '\t' << specularity.y << '\t'
            << specularity.z << '\n';
}

}  // namespace sre