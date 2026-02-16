#pragma once

#include <maddy/parser.h>
#include <myhtml/api.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

class DistBuilder {
   public:
    DistBuilder(fs::path projectRoot);
    ~DistBuilder();

    struct FrontmatterSplit {
        std::string yaml;
        std::string markdown;
    };

    struct HTMLTree {
        myhtml_tree_t *tree;

        HTMLTree(myhtml_t *htmlParser, const std::string &src) {
            tree = myhtml_tree_create();
            myhtml_tree_init(tree, htmlParser);
            myhtml_parse(tree, MyENCODING_UTF_8, src.c_str(), src.size());
        }

        ~HTMLTree() { myhtml_tree_destroy(tree); }

        void Print();
        void PrintNode(myhtml_tree_node_t *node, size_t inc);

        static void PrintNodeAttrs(myhtml_tree_node_t *node);
    };

    struct HTML {
        fs::path path;
        std::unique_ptr<HTMLTree> innerHTML;
    };

    struct Page {
        std::string route;
        std::unique_ptr<HTMLTree> innerHTML;
        YAML::Node pageData;
    };

   private:
    std::string toPermalink(const fs::path &root, const fs::path &file);

    Page parsePage(const fs::path &path);
    std::pair<std::string, HTML> readHTML(const fs::path &path);

    FrontmatterSplit ReadSplitFrontmatter(const fs::path &path);

    fs::path projectRoot;
    YAML::Node projectConfig;

    fs::path pagesPath;
    fs::path layoutsPath;
    fs::path partialsPath;
    fs::path publicPath;

    std::vector<Page> pages;
    std::unordered_map<std::string, HTML> layouts;
    std::unordered_map<std::string, HTML> partials;

    YAML::Node globalData;

    std::shared_ptr<maddy::Parser> mdParser;
    myhtml_t *htmlParser;
};
