#include <filesystem>
#include <fstream>

#include "PFSUtil.h"

#include "PFSConfig.h"
#include "../ext/json.hpp"

using json = nlohmann::json;

bool PFSConfig::LoadConfig(const char *path_to_config) {

    std::ifstream f(path_to_config);
    json data = json::parse(f);

    // Load Name
    this->app_name = data.at("name").get<std::string>();
    printf("Loaded INI Config for App: %s\n",this->app_name.c_str());
    auto ignore_paths = data.at("path_ignore");

    // Load Paths to Ignore
    for (json::iterator it = ignore_paths.begin(); it != ignore_paths.end(); ++it) {
        paths_to_ignore.push_back(*it);
    }

    printf("Paths to Ignore: \n");
    for(const auto& igpath:this->paths_to_ignore){
        printf("%s\n",igpath.c_str());
    }

    // Setting up Map Items
    auto map_items = data.at("map");
    for (auto & map_item : map_items) {
        auto* mi = new MapInstruction ();
        mi->map_src = map_item.at("src").get<std::string>();
        mi->map_target = map_item.at("target").get<std::string>();
        mi->case_sensitive = map_item.at("case_sensitive").get<bool>();
        mi->read_only = map_item.at("read_only").get<bool>();
        this->maps.push_back(mi);
    }

    printf("Skipping EXEFS for now...\n");
    printf("Skipping EntryPoints for now...\n");

    return true;
}
