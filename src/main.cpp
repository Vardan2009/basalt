
#include <iostream>

#include "builder.h"
#include "defs.h"
#include "tty.h"

#if defined(_MSC_VER)

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#endif

int main(int argc, char *argv[]) {
#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    bool help = false, version = false;
    std::string dir;

    if (argc != 2) {
        tty::err("Invalid arguments. Try -h or --help");
        return 1;
    }

    std::string arg = argv[1];

    if (arg == "-h" || arg == "--help")
        help = true;
    else if (arg == "-v" || arg == "--version")
        version = true;
    else
        dir = arg;

    if (help) {
        std::cout << "basalt < -h | -v | (directory) >\n";
        std::cout << "  -h | --help     Shows this page\n";
        std::cout << "  -v | --version  Shows version info\n";
        std::cout << "  (directory)     Project directory to build (root of basalt.yml)\n";
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

    DistBuilder builder(dir);

    builder.BuildWebsite();

    return 0;
}
