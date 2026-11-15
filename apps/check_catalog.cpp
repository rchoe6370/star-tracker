#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include "startracker/catalog.hpp"
#include "startracker/frames.hpp"

int main(int argc, char** argv) {
  const std::string path = (argc > 1) ? argv[1] : "data/catalog.csv";
  try {
    const auto cat = st::Catalog::load(path);
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Loaded " << cat.size() << " stars (mag <= 5.5)\n";

    double worst = 0.0;
    for (const auto& s : cat.stars()) worst = std::max(worst, std::abs(s.dir_sky.norm() - 1.0));
    std::cout << "Max | |v| - 1 | = " << std::scientific << worst << std::fixed << "\n";

    const st::Star* polaris = cat.find(424);
    if (polaris)
      std::cout << "Polaris (HR 424): " << polaris->dir_sky.transpose() << "\n";
    else
      std::cout << "Polaris (HR 424) NOT FOUND\n";

    const st::Star* dubhe = cat.find(4301);
    const st::Star* merak = cat.find(4295);
    if (dubhe && merak)
      std::cout << "Dubhe-Merak angle: "
                << st::rad2deg(st::angle_between(dubhe->dir_sky, merak->dir_sky)) << " deg\n";
    else
      std::cout << "Dubhe (4301) or Merak (4295) NOT FOUND\n";
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
