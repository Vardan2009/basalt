#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>

namespace fs = std::filesystem;

class DistBuilder {
   public:
    DistBuilder(fs::path projectRoot);

   private:
    fs::path projectRoot;
    YAML::Node projectConfig;

    fs::path pagesPath;
    fs::path layoutsPath;
    fs::path partialsPath;
    fs::path publicPath;
};
