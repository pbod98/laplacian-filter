#include <inttypes.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <array>

using namespace std;

struct BMPFileHeader {
    uint16_t file_type = 0;               
    uint32_t file_size = 0;               
    uint16_t reserved1 = 0;               
    uint16_t reserved2 = 0;
    uint32_t offset_data = 0;              
};
struct BMPInfoHeader 
{
    uint32_t size = 0;                      
    uint32_t width = 0;                     
    uint32_t height = 0;                    
                                            
                                            
    uint16_t planes = 0;                    
    uint16_t bits_per_pixel = 0;            
    uint32_t compression = 0;               
    uint32_t padded_image_size = 0;         
    uint32_t x_pixels_per_meter = 0;        
    uint32_t y_pixels_per_meter = 0;
    uint32_t colors_used = 0;               
    uint32_t colors_important = 0;                 
};
struct BMP 
{
    BMPFileHeader file_header;
    BMPInfoHeader info_header;
    uint32_t row_size;
    char* data;

    BMP(const char* fname)
    {
        if (fname != nullptr)
        {
            read(fname);
        }
    }
    void read(const char* fname)
    {
        FILE* file = fopen(fname, "rb");

        if (!file)
        {
            cout << "error opening file";
            return;
        }

        char* header = new char[54];
        fread(header, sizeof(unsigned char), 54, file);

        file_header.file_type = *reinterpret_cast<uint32_t*>(&header[0]);
        file_header.file_size = *reinterpret_cast<uint32_t*>(&header[2]);
        file_header.reserved1 = *reinterpret_cast<uint32_t*>(&header[6]);
        file_header.reserved2 = *reinterpret_cast<uint32_t*>(&header[8]);
        file_header.offset_data = *reinterpret_cast<uint32_t*>(&header[10]);

        info_header.size = *reinterpret_cast<uint32_t*>(&header[14]);
        info_header.width = *reinterpret_cast<uint32_t*>(&header[18]);
        info_header.height = *reinterpret_cast<uint32_t*>(&header[22]);
        info_header.planes = *reinterpret_cast<uint32_t*>(&header[26]);
        info_header.bits_per_pixel = *reinterpret_cast<uint32_t*>(&header[28]);
        info_header.compression = *reinterpret_cast<uint32_t*>(&header[30]);
        info_header.padded_image_size = *reinterpret_cast<uint32_t*>(&header[34]);
        info_header.x_pixels_per_meter = *reinterpret_cast<uint32_t*>(&header[38]);
        info_header.y_pixels_per_meter = *reinterpret_cast<uint32_t*>(&header[42]);
        info_header.colors_used = *reinterpret_cast<uint32_t*>(&header[46]);
        info_header.colors_important = *reinterpret_cast<uint32_t*>(&header[50]);

        row_size = ((info_header.bits_per_pixel * info_header.width + 31) / 32) * 4;
        data = new char[row_size * info_header.height];
        
        fread(data, sizeof(char), row_size * info_header.height, file);       
    } 
     void save(char* data, BMPFileHeader fh, BMPInfoHeader ih, const char* name)
     {
         FILE* result = fopen(name, "wb");
         char header[54];

         header[0] = static_cast<char>(file_header.file_type);
         header[1] = static_cast<char>(file_header.file_type >> 8);
         header[2] = static_cast<char>(file_header.file_size);
         header[3] = static_cast<char>(file_header.file_size >> 8);
         header[4] = static_cast<char>(file_header.file_size >> 16);
         header[5] = static_cast<char>(file_header.file_size >> 24);
         header[6] = static_cast<char>(file_header.reserved1);
         header[7] = static_cast<char>(file_header.reserved1 >> 8);
         header[8] = static_cast<char>(file_header.reserved2);
         header[9] = static_cast<char>(file_header.reserved2 >> 8);
         header[10] = static_cast<char>(file_header.offset_data);
         header[11] = static_cast<char>(file_header.offset_data >> 8);
         header[12] = static_cast<char>(file_header.offset_data >> 16);
         header[13] = static_cast<char>(file_header.offset_data >> 24);
         
         header[14] = (char)(info_header.size);                 
         header[15] = (char)(info_header.size >> 8);            
         header[16] = (char)(info_header.size >> 16);           
         header[17] = (char)(info_header.size >> 24);           
         header[18] = static_cast<char>(info_header.width);
         header[19] = static_cast<char>(info_header.width >> 8);
         header[20] = static_cast<char>(info_header.width >> 16);
         header[21] = static_cast<char>(info_header.width >> 24);
         header[22] = static_cast<char>(info_header.height);
         header[23] = static_cast<char>(info_header.height >> 8);
         header[24] = static_cast<char>(info_header.height >> 16);
         header[25] = static_cast<char>(info_header.height >> 24);
         header[26] = static_cast<char>(info_header.planes);
         header[27] = static_cast<char>(info_header.planes >> 8);
         header[28] = static_cast<char>(info_header.bits_per_pixel);
         header[29] = static_cast<char>(info_header.bits_per_pixel >> 8);
         header[30] = static_cast<char>(info_header.compression);
         header[31] = static_cast<char>(info_header.compression >> 8);
         header[32] = static_cast<char>(info_header.compression >> 16);
         header[33] = static_cast<char>(info_header.compression >> 24);
         header[34] = static_cast<char>(info_header.padded_image_size);
         header[35] = static_cast<char>(info_header.padded_image_size >> 8);
         header[36] = static_cast<char>(info_header.padded_image_size >> 16);
         header[37] = static_cast<char>(info_header.padded_image_size >> 24);
         header[38] = static_cast<char>(info_header.x_pixels_per_meter);
         header[39] = static_cast<char>(info_header.x_pixels_per_meter >> 8);
         header[40] = static_cast<char>(info_header.x_pixels_per_meter>> 16);
         header[41] = static_cast<char>(info_header.x_pixels_per_meter >> 24);
         header[42] = static_cast<char>(info_header.y_pixels_per_meter);
         header[43] = static_cast<char>(info_header.y_pixels_per_meter >> 8 );
         header[44] = static_cast<char>(info_header.y_pixels_per_meter >> 16);
         header[45] = static_cast<char>(info_header.y_pixels_per_meter >> 24);
         header[46] = static_cast<char>(info_header.colors_used);
         header[47] = static_cast<char>(info_header.colors_used >> 8);
         header[48] = static_cast<char>(info_header.colors_used >> 16);
         header[49] = static_cast<char>(info_header.colors_used >> 24);
         header[50] = static_cast<char>(info_header.colors_important);
         header[51] = static_cast<char>(info_header.colors_important >> 8);
         header[52] = static_cast<char>(info_header.colors_important >> 16);
         header[53] = static_cast<char>(info_header.colors_important >> 24);

         fwrite(header, sizeof(char), 54, result);
         fwrite(data, sizeof(char), row_size * info_header.height, result);
     }
};
