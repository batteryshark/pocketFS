#pragma once
#include <string>
#include <vector>

typedef struct MapInstruction{
    std::string map_src;
    std::string map_target;
    bool case_sensitive=false;
    bool read_only=true;
}MapInstruction;

class PFSConfig {
public:
    std::string app_name;
    std::vector<MapInstruction*> maps;
    std::vector<std::string> paths_to_ignore;
    bool LoadConfig(const char* path_to_config);
    PFSConfig()=default;
};
