#include "zip.h"

ZIP::ZIP(std::string path)
{
    this->path = path;
    Read();
}
void ZIP::Read()
{
    std::ifstream input_file(ZIP::path, std::ios::binary);
    LocalFileHeader local_file_header;
    input_file.read(reinterpret_cast<char *>(&local_file_header), size_struct);
    char filename[local_file_header.filename_length];
    char extraField[local_file_header.extra_field_length];
    input_file.read(&filename[0], sizeof(filename));
    input_file.read(&extraField[0], sizeof(extraField));
    local_file_header.file_name = filename;
    local_file_header.extra_field = extraField;
}