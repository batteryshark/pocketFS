#include <stdio.h>
#include <dlfcn.h>

typedef void tPrintInfo();
typedef int tPFSInit(const char* path_to_ini,const char* persistence_base);

static tPrintInfo* PrintInfo = NULL;
static tPFSInit* PFSInit = NULL;

int bind_pfs(){
    void* hdl = dlopen("libpfs.dylib",RTLD_NOW);
    if(hdl){
        printf("Opened dylib successfully!\n");
    }else{
        printf("Fuck\n");
        return 0;
    }
    PrintInfo = dlsym(hdl,"PrintInfo");
    PFSInit = dlsym(hdl,"PFSInit");
    if(!PFSInit || !PrintInfo){
        printf("Error Binding Against Functions in libpfs\n");
        return 0;
    }
    return 1;
}


int main(){
    printf("HI!\n");
    if(!bind_pfs()){
        return -1;
    }
    printf("LibPFS Loaded!\n");

    if(!PFSInit("test_config.json","/Users/batteryshark/test_persistence")){
        printf("PFS Init Failed!\n");
        return -1;
    }

    PrintInfo();

    printf("OK!\n");

    return 0;
}