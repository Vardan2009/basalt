#include "builder.h"

#include <yaml-cpp/yaml.h>

#include <fstream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

#include "tty.h"

DistBuilder::DistBuilder(fs::path projectRoot) : projectRoot(projectRoot) {
    tty::log("Initializing DistBuilder...");
    tty::log("Project Root: {}", projectRoot.generic_string());

    mdParser = std::make_shared<maddy::Parser>();
    htmlParser = myhtml_create();
    myhtml_init(htmlParser, MyHTML_OPTIONS_DEFAULT, 1, 0);

    try {
        projectConfig = YAML::LoadFile((projectRoot / "basalt.yml").string())["site"];
    } catch (const YAML::BadFile &e) {
        tty::err("Error reading `basalt.yml`! {}", e.what());
        exit(1);
    }

    if (!projectConfig.IsDefined()) {
        tty::err("`basalt.yml` includes invalid configuration! `site` is not defined");
        exit(1);
    }

    tty::log("Project Config Loaded:");
    std::cout << projectConfig << '\n';

    globalData = projectConfig["data"];

    if (projectConfig["pages"].IsDefined())
        pagesPath = projectRoot / (projectConfig["pages"].as<std::string>());

    if (projectConfig["layouts"].IsDefined())
        layoutsPath = projectRoot / (projectConfig["layouts"].as<std::string>());

    if (projectConfig["partials"].IsDefined())
        partialsPath = projectRoot / (projectConfig["partials"].as<std::string>());

    if (projectConfig["public"].IsDefined())
        publicPath = projectRoot / (projectConfig["public"].as<std::string>());

    if (projectConfig["out"].IsDefined())
        outPath = projectRoot / (projectConfig["out"].as<std::string>());

    tty::log("Pages:    {}", pagesPath.empty() ? "Not found" : pagesPath.generic_string());
    tty::log("Layouts:  {}", layoutsPath.empty() ? "Not found" : layoutsPath.generic_string());
    tty::log("Partials: {}", partialsPath.empty() ? "Not found" : partialsPath.generic_string());
    tty::log("Public:   {}", publicPath.empty() ? "Not found" : publicPath.generic_string());
    tty::log("Output:   {}\n", outPath.empty() ? "Not found" : outPath.generic_string());

    if (outPath.empty()) {
        tty::err("No output path specified in config! (missing `out`)");
        exit(1);
    }

    if (!pagesPath.empty())
        for (const auto &entry : fs::recursive_directory_iterator(pagesPath)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".md") {
                tty::warn("`{}` is not a Markdown (.md) file, ignoring",
                          entry.path().generic_string());
                continue;
            }

            Page p = parsePage(entry.path());
            pages.push_back(std::move(p));
        }
    else
        tty::warn("No pages path provided in config");

    if (!layoutsPath.empty())
        for (const auto &entry : fs::recursive_directory_iterator(layoutsPath)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".html") {
                tty::warn("`{}` is not an HTML (.html) file, ignoring",
                          entry.path().generic_string());
                continue;
            }

            auto [id, layout] = readHTML(entry.path());

            if (layouts.contains(id)) {
                tty::err("Duplicate layouts with id `{}` found!\n - {}\n - {}", id,
                         layouts[id].path.generic_string(), layout.path.generic_string());
                exit(1);
            }

            layouts[id] = std::move(layout);
        }
    else
        tty::warn("No layouts path provided in config");

    if (!partialsPath.empty())
        for (const auto &entry : fs::recursive_directory_iterator(partialsPath)) {
            if (!entry.is_regular_file()) continue;
            if (entry.path().extension() != ".html") {
                tty::warn("`{}` is not an HTML (.html) file, ignoring",
                          entry.path().generic_string());
                continue;
            }

            auto [id, partial] = readHTML(entry.path());

            if (partials.contains(id)) {
                tty::err("Duplicate partials with id `{}` found!\n - {}\n - {}", id,
                         partials[id].path.generic_string(), partial.path.generic_string());
                exit(1);
            }

            partials[id] = std::move(partial);
        }
}

DistBuilder::~DistBuilder() { myhtml_destroy(htmlParser); }

void DistBuilder::BuildWebsite() {
    if (fs::exists(outPath) && fs::is_directory(outPath)) {
        tty::warn("Output path `{}` is not empty, delete and overwrite? (y/N)",
                  outPath.generic_string());

        char c;
        std::cin >> c;

        if (c != 'y' && c != 'Y') {
            tty::warn("Aborting...");
            exit(0);
        }

        std::error_code ec;
        fs::remove_all(outPath, ec);

        if (ec) {
            tty::err("Error deleting output: {}", ec.message());
            exit(1);
        }
    }

    tty::log("Starting build to `{}`...", outPath.generic_string());
    fs::create_directories(outPath);

    for (const Page &page : pages) {
        fs::path r = page.route;
        fs::path route = outPath / r.relative_path();
        tty::log("Creating `{}`", route.generic_string());

        std::error_code ec;
        fs::create_directories(route, ec);
        if (ec) {
            tty::err("Error creating page for route: {}", ec.message());
            exit(1);
        }

        std::ofstream file(route / "index.html");

        // only serialize page content for now
        // page.innerHTML->SerializeTo(file);
        std::string layout = page.pageData["layout"].as<std::string>();

        HTMLTree final = layouts[layout].innerHTML->Preprocess(page, globalData);
        final.SerializeTo(file);
    }

    if (!publicPath.empty()) {
        tty::log("Copying public `{}`...", publicPath.generic_string());

        for (const auto &entry : fs::directory_iterator(publicPath)) {
            std::error_code ec;

            fs::copy(entry.path(), outPath / entry.path().filename(),
                     fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);

            if (ec) {
                tty::err("Error copying `{}` to `{}`: {}", entry.path().generic_string(),
                         (outPath / entry.path().filename()).generic_string(), ec.message());
                exit(1);
            }
        }
    }
}

static YAML::Node ResolveYAMLPath(const YAML::Node &root, const std::string &path) {
    YAML::Node current = YAML::Clone(root);
    std::string token;

    size_t i = 0;
    auto fail = []() { return YAML::Node(); };

    while (i < path.size()) {
        char c = path[i];

        if (c == '.') {
            if (!token.empty()) {
                YAML::Node next = current[token];
                if (!next || !next.IsDefined()) return fail();

                current = next;
                token.clear();
            }
            ++i;
        } else if (c == '[') {
            if (!token.empty()) {
                YAML::Node next = current[token];
                if (!next || !next.IsDefined()) return fail();

                current = next;
                token.clear();
            }

            ++i;
            if (i >= path.size()) return fail();

            std::string indexStr;
            while (i < path.size() && path[i] != ']') indexStr += path[i++];

            if (i >= path.size() || path[i] != ']') return fail();

            ++i;

            int idx;
            try {
                idx = std::stoi(indexStr);
            } catch (...) {
                return fail();
            }

            YAML::Node next = current[idx];
            if (!next || !next.IsDefined()) return fail();

            current = next;
        } else {
            token += c;
            ++i;
        }
    }

    if (!token.empty()) {
        YAML::Node next = current[token];
        if (!next || !next.IsDefined()) return fail();

        current = next;
    }

    return current;
}

std::string DistBuilder::PreprocessText(const std::string &src, const YAML::Node &pageData,
                                        const YAML::Node &globalData) {
    static const std::regex placeholder(R"(\{\{\s*([\w.\[\]-]+)\s*\}\})");

    std::string result;
    result.reserve(src.size());

    auto it = std::sregex_iterator(src.begin(), src.end(), placeholder);
    auto end = std::sregex_iterator();
    size_t lastPos = 0;

    for (; it != end; ++it) {
        const std::smatch &match = *it;

        result.append(src, lastPos, match.position() - lastPos);
        lastPos = match.position() + match.length();

        const std::string key = match[1].str();

        YAML::Node resolved = ResolveYAMLPath(pageData, key);
        if (resolved.IsNull()) {
            resolved = ResolveYAMLPath(globalData, key);
        }

        if (resolved && resolved.IsDefined() && resolved.IsScalar()) {
            result += resolved.as<std::string>();
        } else {
            result += match[0].str();
        }
    }

    result.append(src, lastPos, std::string::npos);
    return result;
}

std::string DistBuilder::toPermalink(const fs::path &root, const fs::path &file) {
    fs::path absRoot = fs::weakly_canonical(root);
    fs::path absFile = fs::weakly_canonical(file);

    fs::path rel = fs::relative(absFile, absRoot);

    rel.replace_extension("");

    std::string url = "/" + rel.generic_string();

    if (url.size() >= 6 && url.substr(url.size() - 6) == "/index") url.erase(url.size() - 6);

    if (url.empty()) url = "/";

    return url;
}

DistBuilder::Page DistBuilder::parsePage(const fs::path &path) {
    std::string route = toPermalink(pagesPath, path);

    FrontmatterSplit dat = ReadSplitFrontmatter(path);
    std::stringstream input(dat.markdown);
    std::string htmlSrc = mdParser->Parse(input);

    auto innerHTML = std::make_unique<HTMLTree>(htmlParser, htmlSrc, true);
    innerHTML->Print();

    YAML::Node pageData = YAML::Load(dat.yaml);

    return Page{route, std::move(innerHTML), pageData};
}

std::pair<std::string, DistBuilder::HTML> DistBuilder::readHTML(const fs::path &path) {
    std::ifstream file(path);
    if (!file) {
        tty::err("(in {}) Failed to open file", path.generic_string());
        exit(1);
    }
    std::string htmlSrc;

    std::string line;

    while (std::getline(file, line)) htmlSrc += line;

    auto innerHTML = std::make_unique<HTMLTree>(htmlParser, htmlSrc, false);

    innerHTML->Print();

    return {path.stem().generic_string(), HTML{path, std::move(innerHTML)}};
}

DistBuilder::FrontmatterSplit DistBuilder::ReadSplitFrontmatter(const fs::path &path) {
    std::ifstream file(path);
    if (!file) {
        tty::err("(in {}) Failed to open file", path.generic_string());
        exit(1);
    }

    std::string line;

    if (!std::getline(file, line) || line != "---") {
        tty::err("(in {}) Invalid Markdown page structure. First line must be `---`",
                 path.generic_string());
        exit(1);
    }

    std::string yaml;
    std::string markdown;

    bool found_end = false;

    while (std::getline(file, line)) {
        if (line == "---") {
            found_end = true;
            break;
        }
        yaml += line;
        yaml += '\n';
    }

    if (!found_end) {
        tty::err("(in {}) Invalid Markdown page structure. YAML frontmatter not terminated",
                 path.generic_string());
        exit(1);
    }

    std::string rest;
    while (std::getline(file, line)) {
        markdown += line;
        if (!file.eof()) markdown += '\n';
    }

    return FrontmatterSplit{std::move(yaml), std::move(markdown)};
}

void DistBuilder::HTMLTree::SerializeTo(std::ofstream &f) const {
    mycore_string_raw_t str_raw;
    mycore_string_raw_clean_all(&str_raw);

    if (myhtml_serialization_tree_buffer(myhtml_tree_get_document(tree), &str_raw)) {
        tty::err("Could not serialization for the tree");
        exit(1);
    }

    f << str_raw.data;

    mycore_string_raw_destroy(&str_raw, false);
}

DistBuilder::HTMLTree DistBuilder::HTMLTree::Preprocess(const Page &page,
                                                        const YAML::Node &globalData) {
    myhtml_tree_node_t *n = myhtml_node_child(myhtml_tree_get_document(tree));

    myhtml_tree_t *ntree = myhtml_tree_create();
    myhtml_tree_init(ntree, htmlParser);

    myhtml_tree_node_t *ntreed = myhtml_tree_get_document(ntree);

    while (n) {
        myhtml_tree_node_t *node = myhtml_node_clone_deep(ntree, n);

        if (PreprocessNode(ntree, node, page, globalData))
            myhtml_node_append_child(ntreed, node);
        else
            myhtml_node_delete_recursive(node);

        n = myhtml_node_next(n);
    }

    HTMLTree t(htmlParser, ntree);
    t.Print();

    return t;
}

bool DistBuilder::HTMLTree::PreprocessNode(myhtml_tree_t *tree, myhtml_tree_node_t *node,
                                           const Page &page, const YAML::Node &globalData) {
    myhtml_tag_id_t id = myhtml_node_tag_id(node);
    const char *tag = myhtml_tag_name_by_id(tree, id, NULL);

    if (id == MyHTML_TAG__COMMENT) return false;

    if (const char *text = myhtml_node_text(node, NULL)) {
        std::string processed = DistBuilder::PreprocessText(text, page.pageData, globalData);
        myhtml_node_text_set(node, processed.c_str(), processed.size(), myhtml_encoding_get(tree));
    }

    myhtml_tree_attr_t *attr = myhtml_node_attribute_first(node);

    while (attr) {
        const char *key = myhtml_attribute_key(attr, NULL);
        mycore_string_t *str = myhtml_attribute_value_string(attr);

        std::string processed = DistBuilder::PreprocessText(str->data, page.pageData, globalData);

        mycore_string_clean(str);
        mycore_string_append(str, processed.c_str(), processed.size());

        attr = myhtml_attribute_next(attr);
    }

    myhtml_tree_node_t *child = myhtml_node_child(node);
    while (child) {
        myhtml_tree_node_t *next = myhtml_node_next(child);
        if (!PreprocessNode(tree, child, page, globalData)) myhtml_node_delete_recursive(child);
        child = next;
    }

    return true;
}

void DistBuilder::HTMLTree::Print() const {
    myhtml_tree_node_t *node = myhtml_tree_get_document(tree);
    PrintNode(myhtml_node_child(node), 0);
}

void DistBuilder::HTMLTree::PrintNode(myhtml_tree_node_t *node, size_t inc) const {
    while (node) {
        for (size_t i = 0; i < inc; i++) std::cout << '\t';

        const char *tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);
        if (tag_name)
            std::cout << "<'" << tag_name;
        else
            std ::cout << "<!error";

        PrintNodeAttrs(node);

        if (myhtml_node_is_close_self(node)) std ::cout << " /";

        myhtml_tag_id_t tag_id = myhtml_node_tag_id(node);

        if (tag_id == MyHTML_TAG__TEXT || tag_id == MyHTML_TAG__COMMENT) {
            const char *node_text = myhtml_node_text(node, NULL);
            std ::cout << "'>: \"" << node_text << "\"\n";
        } else
            std::cout << "'>\n";

        PrintNode(myhtml_node_child(node), inc + 1);
        node = myhtml_node_next(node);
    }
}

void DistBuilder::HTMLTree::PrintNodeAttrs(myhtml_tree_node_t *node) {
    myhtml_tree_attr_t *attr = myhtml_node_attribute_first(node);

    while (attr) {
        const char *name = myhtml_attribute_key(attr, NULL);

        if (name) {
            std::cout << ' ' << name;

            const char *value = myhtml_attribute_value(attr, NULL);

            std::cout << "=\"" << value << "\"";
        }

        attr = myhtml_attribute_next(attr);
    }
}
