#include "builder.h"

#include <fstream>
#include <memory>

#include "tty.h"

DistBuilder::DistBuilder(fs::path projectRoot) : projectRoot(projectRoot) {
    tty::log("Initializing DistBuilder...");
    tty::log("Project Root: {}", projectRoot.generic_string());

    mdParser = std::make_shared<maddy::Parser>();

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

    tty::log("Pages:    {}", pagesPath.empty() ? "Not found" : pagesPath.generic_string());
    tty::log("Layouts:  {}", layoutsPath.empty() ? "Not found" : layoutsPath.generic_string());
    tty::log("Partials: {}", partialsPath.empty() ? "Not found" : partialsPath.generic_string());
    tty::log("Public:   {}\n", publicPath.empty() ? "Not found" : publicPath.generic_string());

    for (const auto &entry : fs::recursive_directory_iterator(pagesPath)) {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() != ".md") continue;

        pages.push_back(parsePage(entry.path()));
    }
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
    std::string innerHTML = mdParser->Parse(input);

    YAML::Node pageData = YAML::Load(dat.yaml);

    return Page{route, innerHTML, pageData};
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
