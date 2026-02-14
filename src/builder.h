#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <unordered_map>
#include <vector >

namespace fs = std::filesystem;

class DistBuilder {
   public:
    DistBuilder(fs::path projectRoot);

    struct Page {
        std::string route;
        std::string innerHTML;
        YAML::Node pageData;
    };

    struct Layout {
        std::string innerHTML;
    };

    struct Partial {
        std::string innerHTML;
    };

   private:
    std::string toPermalink(const std::filesystem::path &root, const std::filesystem::path &file);

    fs::path projectRoot;
    YAML::Node projectConfig;

    fs::path pagesPath;
    fs::path layoutsPath;
    fs::path partialsPath;
    fs::path publicPath;

    std::vector<Page> pages;
    std::unordered_map<std::string, Layout> layouts;
    std::unordered_map<std::string, Partial> partials;

    YAML::Node globalData;
};
