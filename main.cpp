#include <cstdint>
#include <cstring>
#include <iostream>

bool compare(float a, float b, float c, float d)
{
    return a < b & b < c & c < d;

}

int main()
{
    // std::cout << "Hello, World!" << std::endl;
    // uint32_t dword;
    // float real = 1.0;

    // memcpy(&dword, &real, sizeof(real));

    // std::cout << "dword: " << dword << std::endl;

    float real_1 = 1.0;
    float real_2 = 2.0;
    float real_3 = 3.0;
    float real_4 = 4.0;

    std::cout << compare(real_2, real_1, real_3, real_4) << std::endl;
    
    return 0;
}