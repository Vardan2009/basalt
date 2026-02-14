#include <maddy/parser.h>

#include <iostream>

#include "builder.h"
#include "defs.h"
#include "tty.h"

int main(int argc, char *argv[]) {
    std::cout << tty::fg(tty::Color::BrightBlue)
              << "   ___                ____  | Basalt Static Site Generator\n"
              << "  / _ )___ ____ ___ _/ / /_ | version " << defs::basaltVersion << " (C++"
              << __cplusplus << ")\n"
              << " / _  / _ `(_-</ _ `/ / __/ | " << defs::basaltLink << '\n'
              << "/____/\\_,_/___/\\_,_/_/\\__/  | " << defs::basaltLicenseNote << '\n'
              << '\n'
              << tty::reset();

    // DistBuilder builder("..\\..\\..\\example");

    std::stringstream testInput("# Works!");

    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>();

    std::string htmlOutput = parser->Parse(testInput);

    std::cout << htmlOutput;

    return 0;
}
