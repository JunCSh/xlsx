#include "deflate.h"

Deflate::Deflate(std::string data,unsigned int size_data)
{
    for (int i = 0; i < size_data; i++)
    {
        std::string bin = ConvertIntToBin((int)data[i]);
        int size = bin.size();
        for (int j = 0; j < 8 - size; j++)
            bin += '0';
        bin_data.append(bin);
    }
}
void Deflate::Unpack()
{

}