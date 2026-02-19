
#include <iostream>

#include "builder.h"
#include "defs.h"
#include "tty.h"

#if defined(_MSC_VER)

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

#include <thread>

std::unordered_map<fs::path, fs::file_time_type> files;

void InitializeFileTimes(const fs::path &dir) {
    for (auto &p : fs::recursive_directory_iterator(dir)) {
        if (!fs::is_regular_file(p)) continue;
        files[p.path().string()] = fs::last_write_time(p);
    }
}

bool ScanForChanges(const fs::path &dir, const fs::path &ignoreDir) {
    bool flag = false;

    for (auto &p : fs::recursive_directory_iterator(dir)) {
        if (!fs::is_regular_file(p)) continue;
        if (p.path().string().find(ignoreDir.string()) == 0) continue;

        std::string path = p.path().string();
        auto time = fs::last_write_time(p);

        if (!files.contains(path)) {
            files[path] = time;
            tty::log("New file detected: {}", path);
            flag = true;
        } else if (files[path] != time) {
            files[path] = time;
            tty::log("File modified: {}", path);
            flag = true;
        }
    }

    return flag;
}

int main(int argc, char *argv[]) {
#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    bool help = false, version = false, hotReload = false;
    std::string dir;

    if (argc < 2) {
        version = true;
    } else {
        std::string arg = argv[1];

        if (arg == "-h" || arg == "--help")
            help = true;
        else if (arg == "-v" || arg == "--version")
            version = true;
        else if (arg == "build" && argc == 3)
            dir = argv[2];
        else if (arg == "dev" && argc == 3)
            hotReload = true, dir = argv[2];
        else {
            tty::err("Invalid arguments. Try -h or --help");
            return 1;
        }
    }

    if (help) {
        std::cout << "Usage: basalt <command>\n";
        std::cout << "  -h | --help     Shows this page\n";
        std::cout << "  -v | --version  Shows version info\n";
        std::cout << "  dev <dir>       Start hot reload dev server\n";
        std::cout << "  build <dir>     Build project\n";
        return 0;
    }

    if (version) {
        std::cout << tty::fg(tty::Color::BrightBlue)
                  << "   ___                ____  | Basalt Static Site Generator\n"
                  << "  / _ )___ ____ ___ _/ / /_ | version " << defs::basaltVersion << " (C++"
                  << __cplusplus << ")\n"
                  << " / _  / _ `(_-</ _ `/ / __/ | " << defs::basaltLink << '\n'
                  << "/____/\\_,_/___/\\_,_/_/\\__/  | " << defs::basaltLicenseNote << '\n'
                  << tty::reset();
        return 0;
    }

    if (hotReload) {
        InitializeFileTimes(dir);

        while (true) {
            DistBuilder builder(dir);
            builder.BuildWebsite();
            tty::log("Watching for file changes...");
            while (!ScanForChanges(dir, builder.outPath))
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            tty::log("File change detected. Rebuilding...");
        }
    } else {
        DistBuilder builder(dir);
        builder.BuildWebsite();
    }

    return 0;
}
