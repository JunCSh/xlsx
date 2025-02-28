#include <string> 
#include <cmath>
#include <algorithm> 

std::string ConvertIntToBin(int number,bool reverse = false){
    if(number == 0 ) return "0";
    std::string bin;
    while (number >0){
        bin.append(std::to_string(number%2));
        number /=2;
    }
    if(reverse) std::reverse(bin.begin(), bin.end());
    return bin;
}
int ConvertBinToInt(std::string bin, bool reverse = false){
    if(reverse) std::reverse(bin.begin(), bin.end());
    int num = 0;
    for(int i = 0; i<bin.size();i++){
        if(bin[i] == '1') num += pow(2,i);
    }
    return num;
}