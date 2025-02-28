#include <string>
#include <fstream>

class ZIP
{
private:
    std::string path;
    const std::streamsize size_struct = 30;
    struct LocalFileHeader
    {
        unsigned int signature;                  // Обязательная сигнатура, равна 0x04034b50
        unsigned short version_to_extract;       // Минимальная версия для распаковки
        unsigned short general_purpose_bit_flag; // Битовый флаг
        unsigned short compression_method;       // Метод сжатия (0 -   ез сжатия, 8 - deflate)
        unsigned short modification_time;        // Время модификации файла
        unsigned short modification_date;        // Дата модификации файла
        unsigned int crc32;                      // Контрольная сумма
        unsigned int compressed_size;            // Сжатый размер
        unsigned int uncompressed_size;          // Несжатый размер
        unsigned short filename_length;          // Длина назване файла
        unsigned short extra_field_length;       // Длина поля с дополнительными данными
        char *file_name;
        char *extra_field;
    } __attribute__((packed));
    void Read();
public:
    ZIP(std::string path); 
};