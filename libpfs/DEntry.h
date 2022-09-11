#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "PFSUtil.h"
#include "PFSDevice.h"

#include "PFSData.h"

    class PFSDevice;

    class DEntry {
    private:
        DEntry* parent=nullptr;
        PFSDevice* device;

    public:
        std::string name;

        PFSData* data=nullptr;
        PFSMap<DEntry*>* children = nullptr;

        bool lock=false;
        bool read_only=false;

        [[nodiscard]] bool IsDirectory() const{return this->data == nullptr;}
        // Device Operations
        PFSDevice* GetDevice(){return this->device;}
        void SetDevice(PFSDevice* new_device);
        // Parent Operations
        bool SetParent(DEntry* new_parent);
        DEntry* GetParent();

        // Path Operations
        std::filesystem::path GetFullPath();

        // Child Operations
        [[nodiscard]] size_t CountChildren() const;
        DEntry* GetChild(const char* target_name) const;
        [[nodiscard]] std::vector<DEntry*>* GetChildren() const;
        bool AddChild(DEntry* new_child);
        bool RemoveChild(const char* target_name) const;
        bool RemoveAllChildren() const;
        bool Remove(bool force_remove);

        // Data Operations
        bool CopyDataToPersistence(const char* persistence_base);

        // Constructors / Destructor
        DEntry(const char* new_name, DEntry* new_parent, PFSData* new_data, bool set_read_only);
        DEntry(const char* new_name, DEntry* new_parent, bool set_read_only);
        DEntry(PFSDevice* root_device, bool set_read_only);
        ~DEntry();

    };
