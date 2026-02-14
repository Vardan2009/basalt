#include "builder.h"

#include "tty.h"

DistBuilder::DistBuilder(fs::path projectRoot) : projectRoot(projectRoot) {
    tty::log("Initializing DistBuilder...");
    tty::log("Project Root: {}", projectRoot.generic_string());

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

        tty::log("Page: {} ==> {}", entry.path().generic_string(),
                 toPermalink(pagesPath, entry.path()));
    }
}

std::string DistBuilder::toPermalink(const std::filesystem::path &root,
                                     const std::filesystem::path &file) {
    namespace fs = std::filesystem;

    fs::path absRoot = fs::weakly_canonical(root);
    fs::path absFile = fs::weakly_canonical(file);

    fs::path rel = fs::relative(absFile, absRoot);

    rel.replace_extension("");

    std::string url = "/" + rel.generic_string();

    if (url.size() >= 6 && url.substr(url.size() - 6) == "/index") url.erase(url.size() - 6);

    if (url.empty()) url = "/";

    return url;
}
