#include <stdio.h>
#include <fstream>
#include <iostream>
#include <map>
#include "src\convertor_number_system.h"

extern "C"
{
  int printf(const char *__restrict__ format, ...);
  int scanf(const char *__restrict__ format, ...);
}

struct HaffmanNode
{
  HaffmanNode *left = nullptr;
  HaffmanNode *right = nullptr;
  int *value = nullptr;
  ~HaffmanNode()
  {
    delete left;
    delete right;
    delete value;
  }
};
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
int alphabet_order[]{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
int alphabet[19]{0};
int pointer = 17;

void BuildHaffmanTree(HaffmanNode *head, int i, int len_bit)
{
  if (alphabet[i] > -1)
  {
    std::string bin = ConvertIntToBin(alphabet[i], true);
    int bin_size = bin.size();
    for (int j = 0; j < len_bit - bin_size; j++)
      bin = '0' + bin;
    for (int j = 0; j < bin.size(); j++)
    {
      if (bin[j] == '0')
      {
        if (head->left == nullptr)
        {
          HaffmanNode *new_node = new HaffmanNode;
          head->left = new_node;
          head = new_node;
        }
        else
          head = head->left;
      }
      else
      {
        if (head->right == nullptr)
        {
          HaffmanNode *new_node = new HaffmanNode;
          head->right = new_node;
          head = new_node;
        }
        else
          head = head->right;
      }
    }
    head->value = new int(alphabet_order[i]);
  }
}
void BuildHaffmanTree(HaffmanNode *head, int number, int len_bit, int value)
{

  std::string bin = ConvertIntToBin(number, true);  
  int bin_size = bin.size();
  for (int j = 0; j < len_bit - bin_size; j++)
    bin = '0' + bin;
  for (int j = 0; j < bin.size(); j++)
  {
    if (bin[j] == '0')
    {
      if (head->left == nullptr)
      {
        HaffmanNode *new_node = new HaffmanNode;
        head->left = new_node;
        head = new_node;
      }
      else
        head = head->left;
    }
    else
    {
      if (head->right == nullptr)
      {
        HaffmanNode *new_node = new HaffmanNode;
        head->right = new_node;
        head = new_node;
      }
      else
        head = head->right;
    }
  }
  head->value = new int(value);
}
std::map<int, int> GenerateNextCodes(std::map<int, int> bl_count)
{
  int max_bit = 0;
  for (const auto &p : bl_count)
  {
    max_bit = std::max(max_bit, p.first);
  }
  std::map<int, int> next_code;
  int code = 0;
  for (int bits = 1; bits <= max_bit; bits++)
  {
    int count = 0;
    auto it = bl_count.find(bits - 1);
    if (it != bl_count.end())
      count = it->second;
    code = (code + count) << 1;
    if (bl_count.find(bits) != bl_count.end())
      next_code[bits] = code;
  }
  return next_code;
}

std::string bin_data;
void ReadByHaffmanTree(HaffmanNode *head, int *array, int len)
{
  HaffmanNode *node = head;
  int array_pointer = 0;
  std::string bin;
  while (array_pointer < len)
  {
    bin += bin_data[pointer];
    if (bin_data[pointer] == '0')
      node = node->left;
    else
      node = node->right;
    pointer++;
    if (node->left == nullptr && node->right == nullptr)
    {
      int iii = *node->value;
      if (*node->value < 16)
      {
        array[array_pointer] = *node->value;
        array_pointer++;
      }
      else if (*node->value == 16)
      {
        int len = ConvertBinToInt(bin_data.substr(pointer, 2)) + 3;
        pointer += 2;
        for (int i = 0; i < len; i++)
        {
          array[array_pointer] = array[array_pointer - 1];
          array_pointer++;
        }
      }
      else if (*node->value == 17)
      {
        int len = ConvertBinToInt(bin_data.substr(pointer, 3)) + 3;
        pointer += 3;
        array_pointer += len;
      }
      else if (*node->value == 18)
      {
        int len = ConvertBinToInt(bin_data.substr(pointer, 7)) + 11;
        pointer += 7;
        array_pointer += len;
      }
      node = head;
      bin = "";
    }
  }
}
void ReadByHaffmanTree(HaffmanNode *head, HaffmanNode *head_dist, int len)
{
  const std::map<int, int> code_dist = {
      {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 7}, {6, 9}, {7, 13}, {8, 17}, {9, 25}, {10, 33}, {11, 49}, {12, 65}, {13, 97}, {14, 129}, {15, 193}, {16, 257}, {17, 385}, {18, 513}, {19, 769}, {20, 1025}, {21, 1537}, {22, 2049}, {23, 3073}, {24, 4097}, {25, 6145}, {26, 8193}, {27, 12289}, {28, 16385}, {29, 24577}};
  const std::map<int, int> code_len = {
      {257, 3}, {258, 4}, {259, 5}, {260, 6}, {261, 7}, {262, 8}, {263, 9}, {264, 10}, {265, 11}, {266, 13}, {267, 15}, {268, 17}, {269, 19}, {270, 23}, {271, 27}, {272, 31}, {273, 35}, {274, 43}, {275, 51}, {276, 59}, {277, 67}, {278, 83}, {279, 99}, {280, 115}, {281, 131}, {282, 163}, {283, 195}, {284, 227}, {285, 258}};
  std::string data = "";
  HaffmanNode *node = head;
  std::string bin;
  while (pointer < len)
  {
    bin += bin_data[pointer];
    if (bin_data[pointer] == '0')
      node = node->left;
    else
      node = node->right;
    pointer++;
    if (node->left == nullptr && node->right == nullptr)
    {
      //if(*node->value == 256)return;
      if (*node->value > 256)
      {
        bin = "";
        int len_LZ77 = code_len.at(*node->value);
        if (*node->value > 264 && *node->value != 285)
        {
          int extra_bits = ((*node->value - 257) / 4) - 1;
          len_LZ77 += ConvertBinToInt(bin_data.substr(pointer, extra_bits));
          pointer += extra_bits;
        }
        printf("\x1b[34m");
        printf("%d", len_LZ77);
        node = head_dist;
        while (true)
        {
          bin += bin_data[pointer];
          if (bin_data[pointer] == '0')
            node = node->left;
          else
            node = node->right;
          pointer++;
          if (node->left == nullptr && node->right == nullptr)
          {
            int dist = code_dist.at(*node->value);
            if (*node->value > 3)
            {
              int extra_bits = *node->value / 2 - 1;
              dist += ConvertBinToInt(bin_data.substr(pointer, extra_bits));
              pointer += extra_bits;
            }
            printf("\x1b[35m");
            printf("%d", dist);
            data += data.substr(data.size() - dist, len_LZ77);
            break;
          }
        }
        printf("\x1b[33m");
      }
      else
      {
        char letter = (char)*node->value;
        data += letter;
        printf("%c", letter);
      }
      node = head;
      bin = "";
    }
  }
  std::cout << data;
}

int main()
{
  char path[] = "C:\\Users\\marko\\Downloads\\123.zip";
  std::ifstream input_file(path, std::ios::binary);
  if (!input_file)
  {
    printf("ErorOpen");
    return 1;
  }
  printf("\x1b[33m");
  LocalFileHeader local_file_header;
  input_file.read(reinterpret_cast<char *>(&local_file_header), 30);
  char filename[local_file_header.filename_length];
  char extraField[local_file_header.extra_field_length];
  input_file.read(&filename[0], sizeof(filename));
  input_file.read(&extraField[0], sizeof(extraField));
  local_file_header.file_name = filename;
  local_file_header.extra_field = extraField;
  DeflateHeader deflate;
  unsigned char data[local_file_header.compressed_size];
  input_file.read(reinterpret_cast<char *>(&data), sizeof(data));

  for (int i = 0; i < local_file_header.compressed_size; i++)
  {
    std::string bin = ConvertIntToBin((int)data[i]);
    int size = bin.size();
    for (int j = 0; j < 8 - size; j++)
      bin += '0';
    bin_data.append(bin);
  }
  //bin_data = "1010100010110001100010100101000000000011000001000011000011000010111111111011110010100010011101010110101011100110101110110101000110111010110100001010101110000100011110110111111001010000100111110001000010000100111101001001001101010010111010101101001101001000101000001011101000110111011110110100000100011000011000111100001100010100001100101010000001111010100001100100111011111100110001001011000001010110001111100100011101110011000100111000011110110001101100001100111011101110110111000001001101010000001010011";
  float aaa = bin_data.size();
  printf("%d\n", &local_file_header.signature);
  deflate.BFINAL = ConvertBinToInt(bin_data.substr(0, 1));
  deflate.BTYPE = ConvertBinToInt(bin_data.substr(1, 2));
  DeflateDynamicHuffman deflate_dynamic_huffman;
  if (deflate.BTYPE == 2)
  {
    deflate_dynamic_huffman.HLIT = ConvertBinToInt(bin_data.substr(3, 5)) + 257;
    deflate_dynamic_huffman.HDIST = ConvertBinToInt(bin_data.substr(8, 5)) + 1;
    deflate_dynamic_huffman.HCLEN = ConvertBinToInt(bin_data.substr(13, 4)) + 4;
    std::map<int, int> bl_count;
    bl_count[0] = 0;
    for (int i = 0; i < deflate_dynamic_huffman.HCLEN; i++)
    {
      alphabet[i] = ConvertBinToInt(bin_data.substr(pointer, 3));
      pointer += 3;
      if (alphabet[i] != 0)
      {
        if (bl_count.count(alphabet[i]) != 0)
          bl_count[alphabet[i]] += 1;
        else
          bl_count[alphabet[i]] = 1;
      }
      else
        alphabet[i] = -1;
    }

    std::map<int, int> next_code = GenerateNextCodes(bl_count);

    HaffmanNode *head_codes = new HaffmanNode;
    for (int i = 0; i < deflate_dynamic_huffman.HCLEN; i++)
    {
      if (next_code.count(alphabet[i]) != 0)
      {
        int min = i;
        int cur = alphabet[i];
        for (int j = i + 1; j < deflate_dynamic_huffman.HCLEN; j++)
        {
          if (alphabet[j] == cur && alphabet_order[min] > alphabet_order[j])
            min = j;
        }
        int swap_value = alphabet_order[i];
        alphabet_order[i] = alphabet_order[min];
        alphabet_order[min] = swap_value;

        alphabet[i] = next_code[alphabet[i]];
        BuildHaffmanTree(head_codes, i, cur);
        next_code[cur] += 1;
      }
    }
    int letters[deflate_dynamic_huffman.HLIT]{0};
    ReadByHaffmanTree(head_codes, letters, deflate_dynamic_huffman.HLIT);
    int dist[deflate_dynamic_huffman.HDIST]{0};
    ReadByHaffmanTree(head_codes, dist, deflate_dynamic_huffman.HDIST);
    bl_count.clear();
    for (int i = 0; i < deflate_dynamic_huffman.HLIT; i++)
    {
      if (letters[i] != 0)
      {
        if (bl_count.count(letters[i]) != 0)
          bl_count[letters[i]] += 1;
        else
          bl_count[letters[i]] = 1;
      }
    }
    next_code.clear();
    bl_count[0] = 0;
    next_code = GenerateNextCodes(bl_count);
    HaffmanNode *head_letters = new HaffmanNode;
    for (int i = 0; i < deflate_dynamic_huffman.HLIT; i++)
    {
      if (bl_count.count(letters[i]) != 0 && letters[i] != 0)
      {
        int len_bit = letters[i];
        BuildHaffmanTree(head_letters, next_code[len_bit], len_bit, i);
        letters[i] = next_code[letters[i]];
        next_code[len_bit] += 1;
      }
      else
        letters[i] = -1;
    }

    /////////////////////////////////////////////////////////////////////////
    bl_count.clear();
    for (int i = 0; i < deflate_dynamic_huffman.HDIST; i++)
    {
      if (dist[i] != 0)
      {
        if (bl_count.count(dist[i]) != 0)
          bl_count[dist[i]] += 1;
        else
          bl_count[dist[i]] = 1;
      }
    }
    next_code.clear();
    bl_count[0] = 0;
    next_code = GenerateNextCodes(bl_count);
    HaffmanNode *head_dist = new HaffmanNode;
    
    for (int i = 0; i < deflate_dynamic_huffman.HDIST; i++)
    {
      if (bl_count.count(dist[i]) != 0 && dist[i] != 0)
      {
        int len_bit = dist[i];
        BuildHaffmanTree(head_dist, next_code[len_bit], len_bit, i);

        dist[i] = next_code[dist[i]];
        next_code[len_bit] += 1;
        
      }
      else
        dist[i] = -1;
    }
    ReadByHaffmanTree(head_letters, head_dist, bin_data.size());
    delete head_codes;
    delete head_letters;
    delete head_dist;
  }
  else
  {
    printf("Type dosnt sapport");
    return 1;
  }

  input_file.close();

  scanf("%d");
  return 1;
}