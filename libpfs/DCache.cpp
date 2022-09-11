#include "DCache.h"

    DEntry *DCache::Lookup(const char* target) {
        return (*this->cache)[target];
    }

    void DCache::Add(const char* target, DEntry *n_entry) {
        (*this->cache)[target] = n_entry;
    }

    void DCache::Remove(const char*  target){
        (*this->cache).erase(target);
    }

    DCache::DCache(bool case_sensitive) {
        if(case_sensitive){
            this->cache = reinterpret_cast<PFSMap<DEntry *> *>(new PFSMapCS<DEntry *>());
        }else{
            this->cache = new PFSMap<DEntry*>();
        }
    }


