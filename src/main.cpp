#include <opencv2/opencv.hpp>

#include "../include/Trace.hpp"

using namespace cv;
using namespace sre;

int main() {
  char windName[] = "simple render engine";
  namedWindow(windName, 0);

  Tracer t;
  // t.loadExampleScene();
  t.load("../example/veach-mis/", "veach-mis.obj", "veach-mis.xml");
  // t.load("../example/cornell-box/", "cornell-box.obj", "cornell-box.xml");
  // t.load("../example/staircase/", "stairscase.obj", "staircase.xml");
  auto img = t.render();
  imshow(windName, img);

  waitKey(0);
  destroyWindow(windName);
  return 0;
}