#include "DEntry.h"
#include "DCache.h"
#include "PFSDevice.h"



#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

    PFSMap<DEntry*> PFSDevice::GetAllEntries(DEntry* cnode, fs::path cpath) {
        if (cnode == nullptr) {
            cnode = this->root;
            cpath = "";
        }
        PFSMap<DEntry*> items;
        cpath /= cnode->name;
        // Every item at least gets this entry.
        items[cpath.string()] = cnode;


        if (cnode->IsDirectory()) {
            std::vector<DEntry*>* children = cnode->GetChildren();
            if(children && !children->empty()) {
                for(DEntry* child : *children){
                    PFSMap<DEntry*> children_items = GetAllEntries(child, cpath);
                    for (auto const& [key, val] : (children_items)) {
                        items[key] = val;
                    }
                }
            }
        }
        return items;
    }
#pragma clang diagnostic pop

    PFSDevice::PFSDevice(bool case_sensitive_filesystem, bool read_only_root, const char *device_name) {
        this->d_cache = new DCache(case_sensitive_filesystem);
        if(device_name){
            this->name = device_name;
        }

        this->case_sensitive = case_sensitive_filesystem;
        this->root = new DEntry(this,read_only_root);
    }

    PFSDevice::~PFSDevice() {
        // Kill every node.
        if(this->root){
            this->root->Remove(true);
        }
    }

    // Debug Print Information About Device.
    DeviceInfo* PFSDevice::GetInfo() {
    auto* info = new DeviceInfo ();
    fs::path c_path;
    // Count Directories and Files
    for (auto const& [entry_name, entry] : this->GetAllEntries(nullptr, c_path)) {
        if(entry->IsDirectory()){
            info->num_directories++;
        }else{
            info->num_files++;
        }
        info->entries.push_back(entry->GetFullPath().string());
    }
    return info;
}

DEntry *PFSDevice::QueryPath(const char* in_path) {
    // Check the DCache first - we might have already queried this path before.
    DEntry* current_entry = this->d_cache->Lookup(in_path);
    if(current_entry){
        return current_entry;
    }
    fs::path working_path = PFSUtil::NormalizePath(in_path);

    current_entry = this->root;
    /*
       We Split our given path into parts, and check to see if the current parent has the next part,
       If yes, we move onto the next child until we iterate through the entire path, if it's still found
       at the end, we return the inode, if not, we error out.
    */
    for (auto& part : working_path.relative_path()) {
        // Check if the next part exists as a child to this node.
        current_entry = current_entry->GetChild(part.c_str());
        if(!current_entry){break;}
    }
    // Add a new entry to our DCache
    this->d_cache->Add(in_path,current_entry);
    return current_entry;
}

bool PFSDevice::EntryExists(const char *in_path) {
    return this->QueryPath(in_path) != nullptr;
}

bool PFSDevice::RemoveEntry(const char *in_path, bool force_remove) {
    DEntry* c_entry = this->QueryPath(in_path);
    if(c_entry == nullptr){
        return false;
    }
    // Attempt to remove the entry from dcache at this point.
    this->d_cache->Remove(in_path);
    // Remove the actual entry.
    return c_entry->Remove(force_remove);
}


/*
 Given a path, create a new inode item - by default, this will create a file type.

 By default, if the parent path(s) of the item do not exist, an error will be returned. Directory paths
 can be created with the "create_recursive" flag, provided no files exist in the path that will need to be
 created as a directory.

The "create_directory" flag will create a directory.

If the "link_src_path" parameter is given, this will create a symlink.

*/
bool PFSDevice::AddEntry(const char* target_path, PFSData* file_data, bool create_parent, bool read_only) {
    printf("AddEntry: %s\n",target_path);
    // If we didn't get file data, we're making a directory entry!
    bool create_directory = (file_data == nullptr);
    // We need to make a copy for various reasons, not so much preservation for the caller but also logging.
    fs::path working_path = PFSUtil::NormalizePath(target_path);

    // Clean up our paths and ensure they resolve properly.

    //printf("[PocketFS::Insert]: %s -> %s\n", target_path.string().c_str(), working_path.string().c_str());

    // Find the entry parent.
    fs::path parent_path = working_path.parent_path();

    DEntry* parent_entry = this->QueryPath(parent_path.c_str());
    if (parent_entry == nullptr && !create_parent) {
        // We aren't creating recursively and the parent doesn't exist, so we're out of here!
        return false;
    }


    // Create Parent paths if they don't exist.
    if (create_parent && parent_entry == nullptr) {
        // Select Drive if we don't have a parent.

        parent_entry = this->root;
        DEntry* candidate_entry;

        for (auto& part : parent_path.relative_path()) {
            candidate_entry = parent_entry->GetChild(part.c_str());
            if(!candidate_entry){
                parent_entry->AddChild(new DEntry(part.c_str(),parent_entry,read_only));
            }else if(!candidate_entry->IsDirectory()){
                // We can't have a file as a parent.
                return false;
            }
            parent_entry = parent_entry->GetChild(part.c_str());
        }
    }


    DEntry* target_entry = this->QueryPath(working_path.c_str());
    if (target_entry != nullptr) {
        printf("[AddEntry] Error: Can't insert a preexisting path.\n");
        // We need to return the existing entry if it already exists!
        return true;
    }
    // Create our new entry

    if(create_directory){
        target_entry = new DEntry(working_path.filename().c_str(),parent_entry,read_only);
    }else{
        target_entry = new DEntry(working_path.filename().c_str(), parent_entry, file_data,read_only);
    }

    parent_entry->AddChild(target_entry);
    return true;
}