
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

    std::cout << tty::fg(tty::Color::BrightBlue)
              << "   ___                ____  | Basalt Static Site Generator\n"
              << "  / _ )___ ____ ___ _/ / /_ | version " << defs::basaltVersion << " (C++"
              << __cplusplus << ")\n"
              << " / _  / _ `(_-</ _ `/ / __/ | " << defs::basaltLink << '\n'
              << "/____/\\_,_/___/\\_,_/_/\\__/  | " << defs::basaltLicenseNote << '\n'
              << '\n'
              << tty::reset();

    DistBuilder builder("..\\..\\..\\example");

    builder.BuildWebsite();

    return 0;
}
