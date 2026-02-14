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

    if (projectConfig["pages"].IsDefined())
        pagesPath = projectRoot / (projectConfig["pages"].as<std::string>());

    if (projectConfig["layouts"].IsDefined())
        layoutsPath = projectRoot / (projectConfig["layouts"].as<std::string>());

    if (projectConfig["partials"].IsDefined())
        partialsPath = projectRoot / (projectConfig["partials"].as<std::string>());

    if (projectConfig["public"].IsDefined())
        publicPath = projectRoot / (projectConfig["public"].as<std::string>());

    tty::log("Pages: {}", pagesPath.empty() ? "Not found" : pagesPath.generic_string());
    tty::log("Layouts: {}", layoutsPath.empty() ? "Not found" : layoutsPath.generic_string());
    tty::log("Partials: {}", partialsPath.empty() ? "Not found" : partialsPath.generic_string());
    tty::log("Public: {}", publicPath.empty() ? "Not found" : publicPath.generic_string());
}
