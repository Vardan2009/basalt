#include "builder.h"

#include "tty.h"

DistBuilder::DistBuilder(fs::path projectRoot) : projectRoot(projectRoot) {
    tty::log("Initializing DistBuilder...");
    tty::log("Project Root: {}", projectRoot.generic_string());
}
