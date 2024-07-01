#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <math.h>

#pragma pack(push, 1) // Ensure that structures are byte-aligned
struct BMPHeader {
    uint16_t type; // Magic identifier "BM"
    uint32_t size; // File size
    uint32_t reserved; // Reserved
    uint32_t offset; // Offset to image data
    uint32_t dibHeaderSize; // DIB Header size
    int32_t width; // Image width
    int32_t height; // Image height (positive for bottom-up)
    uint16_t planes; // Number of color planes (must be 1)
    uint16_t bitsPerPixel; // Bits per pixel
    uint32_t compression; // Compression method
    uint32_t imageSize; // Image data size
    int32_t xPixelsPerMeter; // Horizontal pixels per meter
    int32_t yPixelsPerMeter; // Vertical pixels per meter
    uint32_t colorsUsed; // Number of colors in the palette
    uint32_t colorsImportant; // Important colors
};
#pragma pack(pop)

int main(int argc, char *argv[]) {
    
    const char* inFileName = argv[1];
    const char* outFileName = argv[2];
    int times = atof(argv[3]);


    //* Open the input bmp file
    std::ifstream inputFile(inFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "[Error] Failed to open input BMP file." << std::endl;
        return 1;
    }




    //* Read the input bmp header
    BMPHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));

    //* Check if the input bmp file is valid
    if (header.type != 0x4D42) {
        std::cerr << "[Error] Invalid BMP file format." << std::endl;
        inputFile.close();
        return 1;
    }
    int channel = header.bitsPerPixel/8;
    //* Read the input bmp file
    const int imageSize = header.width * header.height * channel;
    std::cout << "[INFO] Input File Width: " << header.width << std::endl;
    std::cout << "[INFO] Input File Height: " << header.height << std::endl;
    std::cout << "[INFO] Input File Channel: " << channel << std::endl;
    std::vector<char> imageData(imageSize);
    inputFile.read(imageData.data(), imageSize);
    inputFile.close();






    std::vector<uint8_t> sharpen_image(imageSize);
  //  for(int i = 0; i<imageSize;i++){
  //      sharpen_image[i] = (uint8_t)imageData[i];
  //  }


    //declare 3x3 sharpen kernel
    int kernel[3][3] = {{-1, -1, -1}, {-1, 9, -1}, {-1, -1, -1}};
    //apply kernel to the image
    for (int loop_time = 0; loop_time<times;loop_time++){
        for (int y = 1; y < header.height - 1; y++) {
            for (int x = 1; x < header.width - 1; x++) {
                for(int ch = 0; ch < channel; ch++){
                    int byte_data=0;
                    for (int ky = -1; ky <= 1; ky++) {
                        for (int kx = -1; kx <= 1; kx++) {
                            byte_data += ((uint8_t)imageData[(((y+ky)*header.width)+ (x+kx))*channel+ch] * kernel[ky + 1][kx + 1]);
                        }
                    }
                    sharpen_image[((y*header.width)+ x)*channel+ch]= (uint8_t)fmin(fmax(byte_data, 0), 255);
                }
            }
        }
    }
    //apply multiple times if need stronger effect
    if(times>1){
        for (int loop_time = 0; loop_time<times-1;loop_time++){
            std::vector<uint8_t> temp_img(sharpen_image);
            for (int y = 1; y < header.height - 1; y++) {
                for (int x = 1; x < header.width - 1; x++) {
                    for(int ch = 0; ch < channel; ch++){
                        int byte_data=0;
                        for (int ky = -1; ky <= 1; ky++) {
                            for (int kx = -1; kx <= 1; kx++) {
                                const int temp =  ((int)temp_img[(((y+ky)*header.width)+ (x+kx))*channel+ch] * kernel[ky + 1][kx + 1]);
                                byte_data += temp;
                            }
                        }
                        sharpen_image[((y*header.width)+ x)*channel+ch]= (uint8_t)fmin(fmax(byte_data, 0), 255);
                    }
                }
            }
        }
    }


    //* Create/Open the output bmp file
    std::ofstream outputFile(outFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "[Error] Failed to create output BMP file." << std::endl;
        return 1;
    }

    //* Output the header and image raw data
    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(BMPHeader));
    outputFile.write((const char *)sharpen_image.data(), imageSize);
    outputFile.close();

    std::cout << "Image is sharpen and saved as "<< outFileName << std::endl;

    return 0;
}