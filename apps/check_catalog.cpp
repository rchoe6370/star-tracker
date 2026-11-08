// Phase 0 sanity checks, printed to the terminal.
#include <iostream>
#include "startracker/catalog.hpp"
#include "startracker/frames.hpp"

int main(int argc, char** argv) {
  const std::string path = (argc > 1) ? argv[1] : "data/catalog.csv";
  try {
    auto cat = st::Catalog::load(path);
    std::cout << "Loaded " << cat.size() << " stars\n";
    // TODO: max | |v|-1 |, Polaris vector, Dubhe-Merak angle in degrees
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
