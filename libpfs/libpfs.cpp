#include <filesystem>
#include "libpfs.h"
#include "PocketFS.h"
#include "PFSHandleManager.h"

namespace fs = std::filesystem;

static PocketFS* pfs;



int PFSInit(const char* path_to_config,const char* persistence_base){
    printf("[PocketFS Initialized!]\n");
    printf("Yes We're Changing Shit\n");

    fs::path test_zip_path = "/Users/batteryshark/Downloads/bt2.zip";
    fs::path zip_base_path = "/app";
    // Test Mapping Zips
    //pfs->MapPath(test_zip_path.c_str(), zip_base_path.c_str(),false, true);
    //std::string persistence_path = persistence_base;
    //pfs->SetPersistencePath(persistence_path);
    pfs = new PocketFS(path_to_config, persistence_base);
    return 1;
}

void PrintInfo(){
    pfs->PrintInfo();
}

/*

 When we open a file handle and it's not redirected, do we:
    -> open a file handle for real in the bypass and keep it in our vfshandle (real_fd and real_handle)
    -> Or do we bury raw file operations down deeper? eventually fseek needs to know to use real operations or not
    -> if you call the fake fseek with a raw file, you're going to hit the hook again, but if you pass it the real fd,
    and relay the info that's all you need.

    The most elegant way to deal with persistence is to just let it make its own fd/file* and bypass everything, only modifying the path.
    Maybe for every non virtualFS approach we need that. If we have a raw that's read only... even if it's not persistent, we can bypass too.

    That essentially means for raw/zip non-persistent write, we instantly persist and then bypass

    Then for zips, read will need the most work because it has to wrap a ton of shit but it's the only one.

    So Basically:
    -> Opening a file as Read-Only?
        -> If zip, virtualized wrapper.
        -> If raw, raw read passthrough with a new path.
        -> If persistence, bypass.
    -> Opening a file as Read-Write?
        -> If raw+persistence, bypass.
        -> If raw/zip non-persistence, copy to persistence + bypass.

    -> Opening a Directory as Read? Read can list children.
        -> Bypass if it doesn't exist, otherwise handle directory read operations via wrappers.

    -> Opening a Directory as Read/Write? Write can add directories.
        -> Basically everything has to be wrapped, but real adds from persistence have to pass through too.

    Need to have a blacklist for writes
    Blacklist for Reads?




 */
