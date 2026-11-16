#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <set>
#include "startracker/catalog.hpp"
#include "startracker/frames.hpp"

using namespace st;
using Catch::Matchers::WithinAbs;

namespace {
constexpr std::int32_t kPolaris = 424;
constexpr std::int32_t kDubhe = 4301;
constexpr std::int32_t kMerak = 4295;

// Load once, reuse in every test.
const Catalog& cat55() {
  static const Catalog c = Catalog::load(ST_CATALOG_PATH, 5.5);
  return c;
}
}  // namespace

TEST_CASE("catalog: every direction is a unit vector") {
  for (const auto& s : cat55().stars())
    CHECK_THAT(s.dir_sky.norm(), WithinAbs(1.0, kTolUnit));
}

TEST_CASE("catalog: Polaris is near the north pole") {
  const Star* p = cat55().find(kPolaris);
  REQUIRE(p != nullptr);
  const double from_pole_deg = rad2deg(angle_between(p->dir_sky, Vec3::UnitZ()));
  INFO("Polaris is " << from_pole_deg << " deg from +z");
  CHECK(from_pole_deg >= 0.70);
  CHECK(from_pole_deg <= 0.78);
  CHECK(p->dir_sky.z() > 0.9999);
}

TEST_CASE("catalog: Dubhe to Merak separation") {
  const Star* d = cat55().find(kDubhe);
  const Star* m = cat55().find(kMerak);
  REQUIRE(d != nullptr);
  REQUIRE(m != nullptr);
  const double sep_deg = rad2deg(angle_between(d->dir_sky, m->dir_sky));
  INFO("Dubhe-Merak = " << sep_deg << " deg");
  CHECK_THAT(sep_deg, WithinAbs(5.4, 0.15));
}

TEST_CASE("catalog: star count at mag 5.5 is in range") {
  const auto n = cat55().size();
  INFO("count = " << n);
  CHECK(n >= 2400);
  CHECK(n <= 3200);
}

TEST_CASE("catalog: magnitude filter") {
  const Catalog cat40 = Catalog::load(ST_CATALOG_PATH, 4.0);
  CHECK(cat40.size() < cat55().size());

  // nothing fainter than the limit
  for (const auto& s : cat40.stars()) CHECK(s.mag <= 4.0);
  for (const auto& s : cat55().stars()) CHECK(s.mag <= 5.5);

  // every star at 4.0 is also present at 5.5
  for (const auto& s : cat40.stars()) CHECK(cat55().find(s.id) != nullptr);
}

TEST_CASE("catalog: no duplicate IDs and lookup works") {
  std::set<std::int32_t> seen;
  for (const auto& s : cat55().stars()) {
    CHECK(seen.insert(s.id).second);        // insert fails if already present
    const Star* f = cat55().find(s.id);
    REQUIRE(f != nullptr);
    CHECK(f->id == s.id);
  }
  CHECK(cat55().find(-1) == nullptr);       // an ID that cannot exist
}