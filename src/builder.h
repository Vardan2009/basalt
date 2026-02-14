#pragma once

#include <maddy/parser.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string_view>
#include <unordered_map>
#include <vector>

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

    struct FrontmatterSplit {
        std::string yaml;
        std::string markdown;
    };

   private:
    std::string toPermalink(const fs::path &root, const fs::path &file);

    Page parsePage(const fs::path &path);

    FrontmatterSplit ReadSplitFrontmatter(const fs::path &path);

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

    std::shared_ptr<maddy::Parser> mdParser;
};
