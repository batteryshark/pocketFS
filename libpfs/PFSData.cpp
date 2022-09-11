#include "PFSData.h"
#include <filesystem>
#include "zip.h"

bool PFSDataZip::ExtractData(const char* output_path) const {
    int error = 0;
    zip_t* zf = zip_open(this->zip_file_path.c_str(), ZIP_RDONLY, &error);
    if (zf == nullptr) {
        printf("[ExtractData] Error Opening Zip File: %d\n", error);
        return false;
    }
    zip_file* cf = zip_fopen(zf, this->zip_data_path.c_str(), 0);
    if (!cf) {
        printf("[ExtractData] Error Opening Path to Data in Zip\n");
        zip_close(zf);
        return false;
    }

    FILE* fd = fopen(output_path, "wb");
    if (!fd) {
        printf("[PFSData::ExtractData] Error Opening Output Path\n");
        zip_close(zf);
        return false;
    }
    size_t chunk_sz = ZIP_BLOCK;
    auto* td = (unsigned char*)calloc(1, chunk_sz);

    size_t bytes_read = 0;
    while (bytes_read < this->zip_data_size) {
        zip_int64_t nr = zip_fread(cf, td, chunk_sz);
        if (nr < 1) {
            zip_close(zf);
            return false;
        }

        fwrite(td, nr, 1, fd);
        bytes_read += nr;
    }

    zip_fclose(cf);
    fclose(fd);
    zip_close(zf);
    return true;
}

bool PFSData::CopyData(const char* destination_path) {
    if(this->raw_path.empty()){return false;}
    if(this->raw_path == destination_path){
        printf("[CopyData]: Error - Source and Destination are the Same!\n");
        return false;
    }
    const auto copyOptions = std::filesystem::copy_options::update_existing;
    std::filesystem::copy(this->raw_path,destination_path,copyOptions);
    return true;
}

PFSData::PFSData(const char* real_path, bool create_read_only){
    this->raw_path = real_path;
    this->is_read_only = create_read_only;
}

bool PFSData::IsRawData() const {
    return !this->raw_path.empty();
}

bool PFSDataZip::CopyData(const char* destination_path) {
    if(!this->ExtractData(destination_path)){
        printf("VFSData_Copy Failed to Extract Data\n");
        return false;
    }
    return true;
}

PFSDataZip::PFSDataZip(const char* n_zip_file_path, const char* n_zip_data_path, size_t data_size,
                       bool is_compressed) {
    this->compressed = is_compressed;
    this->zip_data_size = data_size;
    this->zip_file_path = n_zip_file_path;
    this->zip_data_path = n_zip_data_path;
    this->is_read_only = true;
}


