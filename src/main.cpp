#include "visualizer.h"

int main() {
  Visualizer window(1280, 750, const_cast<char*>("Visualizer"));
  window.Run();
}
