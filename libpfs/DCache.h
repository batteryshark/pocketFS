#pragma once
#include "PFSUtil.h"
#include "DEntry.h"

    class DEntry;

    class DCache {
        PFSMap<DEntry*>* cache{};
    public:
        DEntry* Lookup(const char* target);
        void Add(const char* target, DEntry* n_entry);
        void Remove(const char* target);
        explicit DCache(bool case_sensitive);
        ~DCache(){(*this->cache).clear();}
    };

