#include <filesystem>


#include "PocketFS.h"
#include "PFSHandleManager.h"
#include "PFSData.h"

#include <zip.h>



namespace fs = std::filesystem;

void PocketFS::SetPersistencePath(const char* new_persistence_path) {
    fs::path working_path = new_persistence_path;
    this->persistence_path = absolute(working_path);
    // We'll also map all persistence paths for all devices at this point.
    for (auto const &[key, val]: this->devices) {
        fs::path device_persistence = working_path;

        fs::path virtual_path = val->GetDeviceName();
        if(val->GetDeviceName() != DEFAULT_ROOT_DEVICE_NAME){
            virtual_path += ":/";
            device_persistence / val->GetDeviceName();
        }
        this->MapPath(device_persistence.c_str(),virtual_path.c_str(),val->IsCaseSensitiveFS(),false);
    }

}

PFSDevice *PocketFS::AddDevice(bool is_case_sensitive, bool is_readonly, const char *target_path) {
    // Create Root Node (Directory)
#ifdef _WIN32
    std::string device_name = PFSUtil::GetDriveLetterFromPath(target_path);
    if(device_name.empty()){
        printf("[AddDevice] Error - Unable to Resolve Drive Letter from Path.\n");
        return nullptr;
    }
#else
    std::string device_name = DEFAULT_ROOT_DEVICE_NAME;
#endif
    // If the device already exists, we'll return it.
    if(this->devices[device_name] != nullptr){
        return this->devices[device_name];
    }
    // Otherwise, we'll make a new one with a root and assign it to our global map.
    this->devices[device_name] = new PFSDevice(is_case_sensitive, is_readonly, device_name.c_str());
    return this->devices[device_name];
}

PFSDevice *PocketFS::SelectDevice(const char *target_path) {
// If Windows, we'll attempt to derive a drive letter from the device path.
#ifdef _WIN32
    std::string device_name = Util_GetDriveLetterFromPath(target_path);
    if(device_name.empty() || DeviceMap[device_name] == nullptr){
        printf("[SelectDevice] Error: Device Not Found\n");
        return nullptr;
    }
    return DeviceMap[device_name];
#endif
    // If not Windows, we return the first (and only) device
    for (auto const &[key, val]: this->devices) {
        return val;
    }
    // If we have no devices, we fail.
    return nullptr;
}

void PocketFS::PrintInfo() {
    printf("---[PocketFS Info]---\n");
    for (auto const& [device_name, device] : devices) {
        printf("---[Device: %s]---\n",device->GetDeviceName().c_str());
        DeviceInfo* info = device->GetInfo();
        printf("Number of Directories: %zu\n", info->num_directories);
        printf("Number of Files: %zu\n", info->num_files);
        for(const auto& entry : info->entries) {
            printf("%s\n",entry.c_str());
        }
        delete info;
    }
}

// Given a path to a real directory or zip, map its contents to a virtual root path.
bool PocketFS::MapPath(const char* src_path, const char* virtual_path, bool map_case_sensitive, bool map_read_only) {
    // Determine if we're mapping a raw directory or a zip file.
    fs::path working_path = PFSUtil::NormalizePath(src_path);
    fs::path virtual_base = PFSUtil::NormalizePath(virtual_path);

    // Create a device and root (or find if it already exists).
    PFSDevice* cur_device = this->AddDevice(map_case_sensitive, map_read_only, virtual_base.c_str());

    if(is_directory(working_path)){ // Mapping a Raw Directory
        for (const auto& dirEntry : fs::recursive_directory_iterator(working_path)) {
            // We will end up dirtying this base if we're not careful, make a copy.
            fs::path adjusted_path = virtual_base;
            adjusted_path /= fs::relative(dirEntry.path(), working_path);

            printf("%s => %s\n",dirEntry.path().c_str(),adjusted_path.c_str());
            PFSData* data = nullptr;
            if(!dirEntry.is_directory()){
                data = new PFSData(dirEntry.path().c_str(),map_read_only);
            }


            if (!cur_device->AddEntry(adjusted_path.c_str(),data,true,map_read_only)) {
                printf("[MapRawPath] Error Inserting Path: %s\n", adjusted_path.string().c_str());
                return false;
            }
        }
    }else{ // Map Zip File
        int error = 0;
        zip_t* zf = zip_open(working_path.string().c_str(), ZIP_RDONLY, &error);
        if (zf == nullptr) {
            printf("[map_zip] Error Opening Zip File: %d\n", error);
            return false;
        }
        zip_int64_t num_entries = zip_get_num_entries(zf, 0);

        for (zip_int64_t i = 0; i < num_entries; i++) {
            struct zip_stat stat{};
            zip_stat_index(zf, i, ZIP_FL_ENC_GUESS, &stat);

            bool is_directory = std::string(stat.name).ends_with('/');
            std::string sanitized_string = stat.name;
            PFSData* data = nullptr;
            if(is_directory){
                // We're going to rip out the trailing slash.
                sanitized_string.pop_back();
            }else{
                // We only get a data object for files.
                data = new PFSDataZip(working_path.c_str(),stat.name,stat.size,(stat.size != stat.comp_size));
            }

            fs::path adjusted_path = fs::path(virtual_path) /= sanitized_string;

            if (!cur_device->AddEntry(adjusted_path.c_str(),data,  true,map_read_only)) {
                printf("[MapZip] Error Inserting Path: %s\n", adjusted_path.c_str());
                zip_close(zf);
                return false;
            }


        }
        zip_close(zf);
    }
    return true;
}

PocketFS::PocketFS(const char *path_to_config, const char* persistence_base) {
    this->config = new PFSConfig();
    // Map all mappings
    this->config->LoadConfig(path_to_config);
    for(auto* mi: this->config->maps){
        printf("Mapping: %s => %s %d %d\n",mi->map_src.c_str(),mi->map_target.c_str(),mi->case_sensitive,mi->read_only);
        this->MapPath(mi->map_src.c_str(),mi->map_target.c_str(),mi->case_sensitive,mi->read_only);
    }

    // Deal with Persistence Layer
    fs::path working_persistence_path = persistence_base;
    working_persistence_path /= this->config->app_name;
    this->SetPersistencePath(working_persistence_path.c_str());

}
