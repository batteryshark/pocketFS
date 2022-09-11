#pragma once
#include <string>
#include <map>

#define HANDLE_INDEX_START 0x1000
#define HANDLE_INDEX_END 0x2000
#define HANDLE_MAGIC "LIB-PFS"

typedef struct PFSHandle{
    const char* magic = HANDLE_MAGIC;
    size_t read_offset = 0;
    size_t write_offset = 0;
    int operation_mode = 0;
    bool read_only = true;
    void* dentry = nullptr;
    int32_t real_fd = -1;
    void* real_handle = nullptr;
}PFSHandle;


class PFSHandleManager{
private:
    std::map<std::int32_t, PFSHandle*> fd_map;
public:
    bool Create(void* new_entry, PFSHandle* phandle);
    bool Create(void* new_entry, int32_t* fd);
    bool Remove(PFSHandle* handle);
    bool Remove(int32_t fd);
    bool GetDescriptor(PFSHandle* handle, int32_t* fd);
    static bool IsPFSHandle(int32_t fd);
    static bool IsPFSHandle(struct PFSHandle* handle);
};





