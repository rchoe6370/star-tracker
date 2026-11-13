#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "startracker/catalog.hpp"
#include "startracker/frames.hpp"

using namespace st;
using Catch::Matchers::WithinAbs;

constexpr int kPolaris = 424;
constexpr int kDubhe = 4301;
constexpr int kMerak = 4295;

TEST_CASE("catalog sanity") {
  auto cat = Catalog::load(ST_CATALOG_PATH);
  REQUIRE(cat.size() > 1000);

  SECTION("all vectors are unit length") {
    for (const auto& s : cat.stars())
      REQUIRE_THAT(s.v_sky.norm(), WithinAbs(1.0, 1e-9));
  }
  SECTION("Polaris is near the north pole") {
    const Star* p = cat.find(kPolaris);
    REQUIRE(p != nullptr);
    CHECK(p->v_sky.z() > 0.999);
  }
  SECTION("Dubhe-Merak is about 5.5 degrees") {
    const Star* d = cat.find(kDubhe);
    const Star* m = cat.find(kMerak);
    REQUIRE(d != nullptr);
    REQUIRE(m != nullptr);
    CHECK_THAT(rad2deg(angle_between(d->v_sky, m->v_sky)), WithinAbs(5.5, 0.2));
  }
}
