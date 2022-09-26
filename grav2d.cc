#include "colors.h"
#include "engine.h"
#include "planet_2d.h"
#include "vector_force_2d.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <gflags/gflags.h>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/reader.h>
#include <spdlog/spdlog.h>

DEFINE_string(universal_conf, "universe.json", "a json defines planets");
DEFINE_uint32(draw_interval, 5, "in multiple of 100 milliseconds");
DEFINE_bool(draw_aux, false, "draw auxilary lines");

std::unique_ptr<spdlog::logger> grav2d_logger;

int main(int argc, char **argv) {
  gflags::SetUsageMessage("./grav2");
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  if (FLAGS_universal_conf.empty()) {
    return EXIT_FAILURE;
  }
  auto pfile = std::unique_ptr<std::FILE, void (*)(std::FILE *)>(
      fopen(FLAGS_universal_conf.c_str(), "r"), [](auto p) { fclose(p); });
  if (!pfile) {
    return EXIT_FAILURE;
  }

  extern Engine engine;

  rapidjson::Document d;
  const auto kBufSize = 512u;
  char buffer[kBufSize];
  rapidjson::FileReadStream frs(pfile.get(), buffer, kBufSize);

  d.ParseStream<rapidjson::kParseCommentsFlag>(frs);
  const auto &json_planets = d["planets"].GetArray();
  for (const auto &p : json_planets) {
    const auto &pos = p["position"];
    auto pos_x = pos["x"].GetDouble();
    auto pos_y = pos["y"].GetDouble();
    auto radius = pos["radius"].GetDouble();
    const auto &color = p["color"];
    auto redf = color["red"].GetFloat();
    auto greenf = color["green"].GetFloat();
    auto bluef = color["blue"].GetFloat();
    auto alpha = color["alpha"].GetFloat();
    const auto &velocity = p["velocity"];
    auto v_x = velocity["x"].GetDouble();
    auto v_y = velocity["y"].GetDouble();
    double weight = p["weight"].GetDouble();

    Planet2D p2d{.origin = {.x = pos_x, .y = pos_y},
                 .radius = radius,
                 .weight = weight,
                 .color = {redf, greenf, bluef, alpha}};
    VectorForce2D v2d(v_x, v_y);

    engine.AddPlanet(std::move(p2d), std::move(v2d));
  }

  const auto &galaxy_cfg = d["galaxy"];
  auto galaxy_size_x = galaxy_cfg["width"].GetInt();
  auto galaxy_size_y = galaxy_cfg["height"].GetInt();
  const auto &bgcolor_cfg = galaxy_cfg["background"];
  RGBAf bgcolor{bgcolor_cfg["red"].GetFloat(), bgcolor_cfg["green"].GetFloat(),
                bgcolor_cfg["blue"].GetFloat(),
                bgcolor_cfg["alpha"].GetFloat()};

  engine.EngineInit(galaxy_size_x, galaxy_size_y, bgcolor);
  engine.Run();
}