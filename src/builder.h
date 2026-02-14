#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>

namespace fs = std::filesystem;

class DistBuilder {
   public:
    DistBuilder(fs::path projectRoot);

    struct Page {
        std::string route;
        std::string innerHTML;
        YAML::Node pageData;
    };

   private:
    fs::path projectRoot;
    YAML::Node projectConfig;

    fs::path pagesPath;
    fs::path layoutsPath;
    fs::path partialsPath;
    fs::path publicPath;

    std::vector<Page> pages;
};
