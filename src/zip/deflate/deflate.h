#include <string>
#include "../convertor_number_system.h"

class Deflate
{
private:
    std::string bin_data;
    struct DeflateHeader
    {
        unsigned short BFINAL;
        unsigned short BTYPE;
    };
    struct DeflateDynamicHuffman
    {
        unsigned int HLIT = 0;
        unsigned int HDIST = 0;
        unsigned int HCLEN = 0;
    };

public:
    Deflate(std::string data, unsigned int size_data);
    ~Deflate();
    void Unpack();
};
