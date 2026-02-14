#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class DistBuilder {
   public:
    DistBuilder(fs::path projectRoot);

   private:
    fs::path projectRoot;
};
