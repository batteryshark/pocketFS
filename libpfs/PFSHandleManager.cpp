#include "PFSHandleManager.h"

 bool PFSHandleManager::IsPFSHandle(struct PFSHandle *handle) {
    if(!handle){return false;}
    if(!memcmp(handle,HANDLE_MAGIC,sizeof(HANDLE_MAGIC))){
        return true;
    }
    return false;
}

 bool PFSHandleManager::IsPFSHandle(int32_t fd) {
    if(fd >= HANDLE_INDEX_START && fd <= HANDLE_INDEX_END){
        return true;
    }
    return false;
}

bool PFSHandleManager::Remove(int32_t fd) {
    if(!this->fd_map[fd]){return false;}
    this->fd_map.erase(fd);
    return true;
}

bool PFSHandleManager::Remove(struct PFSHandle *handle) {
    if(this->IsPFSHandle(handle)){return false;}
    int32_t descriptor = 0;
    if(!this->GetDescriptor(handle,&descriptor)){return false;}
    this->fd_map.erase(descriptor);
    delete handle;
    return true;
}

bool PFSHandleManager::Create(void *new_entry, PFSHandle* phandle) {
    for(int32_t n_fd = HANDLE_INDEX_START; n_fd < HANDLE_INDEX_END; n_fd++){
        if(this->fd_map[n_fd] == nullptr){
            phandle = new PFSHandle();
            phandle->dentry = new_entry;
            this->fd_map[n_fd] = phandle;
            return true;
        }
    }
    return false;
}

bool PFSHandleManager::Create(void *new_entry, int32_t *fd) {
    for(int32_t n_fd = HANDLE_INDEX_START; n_fd < HANDLE_INDEX_END; n_fd++){
        if(this->fd_map[n_fd] == nullptr){
            auto* n_handle = new PFSHandle();
            n_handle->dentry = new_entry;
            this->fd_map[n_fd] = n_handle;
            *fd = n_fd;
            return true;
        }
    }
    return false;
}

bool PFSHandleManager::GetDescriptor(PFSHandle* handle, int32_t* fd){
    for (auto const& [key, val] : this->fd_map) {
        if(val == handle){
            *fd = key;
            return true;
        }
    }
    return false;
}


