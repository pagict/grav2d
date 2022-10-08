
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <utility>

#include <gflags/gflags.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/reader.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "colors.h"
#include "engine.h"
#include "planet_2d.h"
#include "vector_force_2d.h"

DEFINE_string(universal_conf, "universe.json", "a json defines planets");
DEFINE_uint32(recalc_millisec, 100, "recalculate interval");
DEFINE_uint32(draw_interval, 5,
              "in multiple of `recalc_millisec` milliseconds");
DEFINE_int64(recalc_cnt, -1,
             "only recalc this times. -1 means infinity update");
DEFINE_string(log_level, "info", "trace|debug|info|warning|error|critical|off");
DEFINE_uint32(density, 1, "the density of a plenty, use for weight calc");
DEFINE_uint32(history_tracks, 100, "history track length");
DEFINE_uint32(predict_cnt, 100, "ticks when predict moving tracks");

unsigned bg_stars = 800;

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
    double density = FLAGS_density;
    if (p.HasMember("density")) {
      density = p["density"].GetDouble();
    }
    auto weight = radius * radius * M_PI * density;

    Planet2D p2d{
        {.x = pos_x, .y = pos_y}, radius, weight, {redf, greenf, bluef, alpha}};
    VectorForce2D v2d(v_x, v_y);

    engine.AddPlanet(std::move(p2d), std::move(v2d));
  }

  auto grav2d_logger = spdlog::basic_logger_st("grav2d", "grav2d.log", true);
  grav2d_logger->set_level(spdlog::level::from_str(FLAGS_log_level));
  grav2d_logger->set_pattern("[%H:%M:%S.%e][%L][%@] %v");
  spdlog::set_default_logger(grav2d_logger);
  SPDLOG_INFO("starting a round...");

  const auto &galaxy_cfg = d["galaxy"];
  auto galaxy_size_x = galaxy_cfg["width"].GetInt();
  auto galaxy_size_y = galaxy_cfg["height"].GetInt();
  const auto &bgcolor_cfg = galaxy_cfg["background"];
  RGBAf bgcolor{bgcolor_cfg["red"].GetFloat(), bgcolor_cfg["green"].GetFloat(),
                bgcolor_cfg["blue"].GetFloat(),
                bgcolor_cfg["alpha"].GetFloat()};
  bg_stars = bgcolor_cfg["stars"].GetUint();

  engine.EngineInit(galaxy_size_x, galaxy_size_y, bgcolor);
  engine.Run();
}