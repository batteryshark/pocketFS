#pragma once

#include "PocketFS.h"


extern "C"{
int PFSInit(const char* path_to_config,const char* persistence_base);
void PrintInfo();
}
