#pragma once
#include <string>

#define ZIP_BLOCK 65535

class PFSData{
public:
    std::string raw_path;
    bool is_read_only=true;
    virtual bool CopyData(const char* destination_path);
    PFSData()= default;
    PFSData(const char* real_path, bool create_read_only);
    [[nodiscard]] bool IsRawData() const;
};

class PFSDataZip:public PFSData{
    bool ExtractData(const char* output_path) const;
public:
    bool compressed = false;
    size_t zip_data_size = 0;
    std::string zip_file_path;
    std::string zip_data_path;
    bool CopyData(const char* destination_path) override;
    PFSDataZip(const char* n_zip_file_path, const char* n_zip_data_path, size_t data_size, bool is_compressed);
};

