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

    void BuildWebsite();

    struct FrontmatterSplit {
        std::string yaml;
        std::string markdown;
    };

    struct Page;

    struct HTMLTree {
        myhtml_tree_t *tree;
        myhtml_t *htmlParser;

        HTMLTree(myhtml_t *htmlParser, myhtml_tree_t *tree) : htmlParser(htmlParser), tree(tree) {}

        HTMLTree(myhtml_t *htmlParser, const std::string &src, bool isFragment)
            : htmlParser(htmlParser) {
            tree = myhtml_tree_create();
            myhtml_tree_init(tree, htmlParser);
            if (isFragment)
                myhtml_parse_fragment(tree, MyENCODING_UTF_8, src.c_str(), src.size(),
                                      MyHTML_TAG_DIV, MyHTML_NAMESPACE_ANY);
            else
                myhtml_parse(tree, MyENCODING_UTF_8, src.c_str(), src.size());
        }

        ~HTMLTree() { myhtml_tree_destroy(tree); }

        void SerializeTo(std::ofstream &f) const;
        HTMLTree Preprocess(const Page &page);

        void PreprocessNode(myhtml_tree_t *tree, myhtml_tree_node_t *node, const Page &page);

        void Print() const;
        void PrintNode(myhtml_tree_node_t *node, size_t inc) const;

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
    fs::path outPath;

    std::vector<Page> pages;
    std::unordered_map<std::string, HTML> layouts;
    std::unordered_map<std::string, HTML> partials;

    YAML::Node globalData;

    std::shared_ptr<maddy::Parser> mdParser;
    myhtml_t *htmlParser;
};
