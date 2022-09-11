#include "DEntry.h"


    DEntry::DEntry(const char *new_name, DEntry *new_parent, PFSData *new_data, bool set_read_only) {
        this->device = new_parent->device;
        this->name = new_name;
        this->parent = new_parent;
        this->data = new_data;
        this->read_only = set_read_only;
    }

    DEntry::DEntry(const char *new_name, DEntry *new_parent, bool set_read_only) {
        this->device = new_parent->device;
        this->name = new_name;
        this->parent = new_parent;
        this->read_only = set_read_only;
        if(this->device != nullptr && this->device->IsCaseSensitiveFS()){
            this->children = reinterpret_cast<PFSMap<DEntry *> *>(new PFSMapCS<DEntry *>());
        }else{
            this->children = new PFSMap<DEntry*>();
        }
    }

    DEntry::DEntry(PFSDevice *root_device, bool set_read_only) {
        this->device = root_device;
        this->name = "/";
        this->read_only = set_read_only;
        if(root_device->IsCaseSensitiveFS()){
            this->children = reinterpret_cast<PFSMap<DEntry *> *>(new PFSMapCS<DEntry *>());
        }else{
            this->children = new PFSMap<DEntry*>();
        }
    }

    std::filesystem::path DEntry::GetFullPath() {
        fs::path out_path = this->name;
        DEntry* ce = this->parent;

        while (ce != nullptr) {
            out_path = ce->name / out_path;
            ce = ce->parent;
        }
// If we're on Windows, add the drive letter prefix from device.
#ifdef _WIN32
        out_path = this->device->name + out_path;
#endif
        return out_path;
    }

    // Gets a list of all children.
    std::vector<DEntry*>* DEntry::GetChildren() const{
        auto* results = new std::vector<DEntry*>();
        if(this->children != nullptr){
            for (auto const& [key, val] : (*this->children)) {
                results->push_back(val);
            }
        }
        return results;
    }

    DEntry *DEntry::GetChild(const char *target_name) const {
        if(this->children == nullptr){return nullptr;}
        return (*this->children)[target_name];
    }

    size_t DEntry::CountChildren() const {
        if(this->children == nullptr){return 0;}
        return (*this->children).size();
    }

    bool DEntry::RemoveAllChildren() const {
        if(!this->IsDirectory()){return false;}
        if(this->children){
            (*this->children).clear();
        }
        return true;
    }

    bool DEntry::RemoveChild(const char *target_name) const {
        if(this->children == nullptr){return false;}
        (*this->children).erase(target_name);
        return true;
    }

    bool DEntry::AddChild(DEntry *new_child) {
        if(!this->IsDirectory()){return false;}
        if(this->children){
            (*this->children)[new_child->name] = new_child;
            // Re-assign device just in case this is a move.
            new_child->device = this->device;
        }
        return true;
    }

    bool DEntry::SetParent(DEntry *new_parent) {
        // A parent can only be a directory.
        if(!new_parent->IsDirectory()){return false;}
        //  If a parent already existed, ensure that you remove the child from that parent first.
        if(this->parent != nullptr){
            this->parent->RemoveChild(this->name.c_str());
        }
        // Add this child node to the parent.
        if(!new_parent->AddChild(this)){return false;}
        // Make the assignment.
        this->parent = new_parent;
        this->SetDevice(new_parent->device);

        return true;
    }

    DEntry *DEntry::GetParent() {
        return this->parent;
    }

    // Update the Device ownership.
    void DEntry::SetDevice(PFSDevice* new_device) {
        if(this->device != new_device){
            if((this->device->IsCaseSensitiveFS() != this->device->IsCaseSensitiveFS()) && this->IsDirectory()){
                // If we moved this entry from a non-case-sensitive Filesystem, we have to reset its children.
                PFSMap<DEntry*>* new_children;
                if(new_device->IsCaseSensitiveFS()){
                    new_children = reinterpret_cast<PFSMap<DEntry *> *>(new PFSMapCS<DEntry *>());
                }else{
                    new_children = new PFSMap<DEntry*>();
                }
                for (auto const& [key, val] : (*this->children)) {
                    (*new_children)[key] = val;
                }
                delete this->children;
                this->children = new_children;
            }
            this->device = new_device;
        }
    }


    /*
   Remove this inode from its parent and delete(de-allocate) it.
   The "force_remove" option will, if this is a directory and not empty,
   also remove the children of this inode.
*/
    bool DEntry::Remove(bool force_remove) { // NOLINT(misc-no-recursion)

        // Directories are a special case - we need to clear out children if the force flag was given.
        if(this->IsDirectory()){
            if(this->children != nullptr){
                if(!(*this->children).empty() && !force_remove){
                    printf("[DEntry::Remove] Error - Directory is not Empty!\n");
                    return false;
                }else{
                    for (auto const& [key, val] : (*this->children)) {
                        val->Remove(true);
                    }
                }
            }
        }

        // Remove Child node on Parent if it exists.
        if(this->parent != nullptr){
            this->parent->RemoveChild(this->name.c_str());
        }

        // ** Danger - Point Away from Face **
        delete this;
        return true;
    }


    // Destroy every node underneath this node.
    DEntry::~DEntry() {
        this->name.clear();
        this->parent = nullptr;
        this->data = nullptr;
        if(this->children){
            (*this->children).clear();
        }
        delete this->children;
    }

bool DEntry::CopyDataToPersistence(const char* persistence_base) {
    fs::path working_path = this->GetFullPath();
    fs::path persistence_path = persistence_base / working_path.relative_path();
    printf("Test Copy to Persistence: %s\n", working_path.string().c_str());
    printf("Persistence Path: %s\n", persistence_path.string().c_str());
    fs::create_directories(persistence_path.parent_path());

    if (!this->data->CopyData(persistence_path.c_str())) {
        printf("Copy Data to Persistence Failed!\n");
        return false;
    }
    // Then, replace the data handler of this inode with the raw handler type.
    delete this->data;
    this->data = new PFSData(persistence_path.c_str(),false);
    return true;
}

