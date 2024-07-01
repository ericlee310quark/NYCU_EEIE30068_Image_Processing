#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <string>

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t colorsUsed;
    uint32_t colorsImportant;
};
#pragma pack(pop)

struct Pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s input.bmp output.bmp \"G\" -> Use Gray World \n %s input.bmp output.bmp \"M\" -> Use Max RGB\n", argv[0],argv[0]);
        return 1;
    }
    const char* filename = argv[1];
    const char* outputFilename = argv[2];
    std::string alg_type = argv[3];
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 1;
    }

    BMPHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (header.signature != 0x4D42) {
        std::cerr << "File is not a BMP!" << std::endl;
        return 1;
    }

    if (header.bitsPerPixel != 24) {
        std::cerr << "Only 24-bit BMPs are supported!" << std::endl;
        return 1;
    }

    std::vector<Pixel> pixels(header.width * header.height);
    file.seekg(header.dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(Pixel));
    file.close();

    //! This is discarded
    if(alg_type.find("G")!=-1){
        // Calculate the average of each channel
        uint8_t gammaTable[256];
        double gamma = 0.25;
        for (int i = 0; i < 256; i++) {
            gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
        }
        for (auto& pixel : pixels) {
            pixel.red = gammaTable[pixel.red];
            pixel.green = gammaTable[pixel.green];
            pixel.blue = gammaTable[pixel.blue];

        }


        double sumR = 0, sumG = 0, sumB = 0;
        for (const auto& pixel : pixels) {
            sumR += pixel.red;
            sumG += pixel.green;
            sumB += (pixel.blue);
        }

        double avgR = sumR / pixels.size();
        double avgG = sumG / pixels.size();
        double avgB = sumB / pixels.size();


        std::cout<<avgB<<std::endl;
        if ((!(avgG>avgR))&&(!(avgB>avgR))){
            double scaleG = avgR / avgG;
            double scaleB = avgR / avgB;
            for (auto& pixel : pixels) {
                pixel.green = static_cast<uint8_t>(std::min(255.0, pixel.green * scaleG));
                pixel.blue = static_cast<uint8_t>(std::min(255.0, pixel.blue * scaleB));
            }

        }
        else if((!(avgR>avgG))&&(!(avgB>avgG))){
            double scaleR = avgG / avgR;
            double scaleB = avgG / avgB;
            for (auto& pixel : pixels) {
                pixel.red = static_cast<uint8_t>(std::min(255.0, pixel.red * scaleR));
                pixel.blue = static_cast<uint8_t>(std::min(255.0, pixel.blue * scaleB));
            }

        }
        else if((!(avgR>avgB))&&(!(avgG>avgB))){
            double scaleR = avgB / avgR;
            double scaleG = avgB / avgG;
            for (auto& pixel : pixels) {
                pixel.red = static_cast<uint8_t>(std::min(255.0, pixel.red * scaleR));
                pixel.green = static_cast<uint8_t>(std::min(255.0, pixel.green * scaleG));
            }

        }

        gamma = 1.75;
        for (int i = 0; i < 256; i++) {
            gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
        }
        for (auto& pixel : pixels) {
            pixel.red = gammaTable[pixel.red];
            pixel.green = gammaTable[pixel.green];
            pixel.blue = gammaTable[pixel.blue];

        }
    }
    // Gray World -> G_avg is the standard light src
    else if(alg_type.find("X")!=-1){

        double sumR = 0, sumG = 0, sumB = 0;
        for (const auto& pixel : pixels) {
            sumR += pixel.red;
            sumG += pixel.green;
            sumB += (pixel.blue);
        }

        double avgR = sumR / pixels.size();
        double avgG = sumG / pixels.size();
        double avgB = sumB / pixels.size();

        double scaleR = avgG / avgR;
        double scaleB = avgG / avgB;
        for (auto& pixel : pixels) {
            pixel.red = static_cast<uint8_t>(std::min(255.0, pixel.red * scaleR));
            pixel.blue = static_cast<uint8_t>(std::min(255.0, pixel.blue * scaleB));
        }
    }
    // MaxRGB -> 255 is the standard light src
    else if(alg_type.find("M")!=-1){
        int max_value = 255;
        int max_b =0, max_g = 0, max_r = 0;
        for (const auto& pixel : pixels) {
            if(max_b<int(pixel.blue)){
                max_b = (pixel.blue);
            }
            if(max_g<int(pixel.green)){
                max_g = int(pixel.green);
            }
            if(max_r<int(pixel.red)){
                max_r = int(pixel.red);
            }
        }
        double kb = double(max_value)/double(max_b);
        double kg = double(max_value)/double(max_g);
        double kr = double(max_value)/double(max_r);
        int temp_b, temp_g, temp_r;

        for (auto& pixel : pixels) {
            temp_b = floor(double(pixel.blue)*kb);
            temp_g = floor(double(pixel.green)*kg);
            temp_r = floor(double(pixel.red)*kr);
            if(temp_b<0){
                temp_b = 0;
            }
            else if(temp_b>255){
                temp_b = 255;
            }
            if(temp_g<0){
                temp_g = 0;
            }
            else if(temp_g>255){
                temp_g = 255;
            }
            if(temp_r<0){
                temp_r = 0;
            }
            else if(temp_r>255){
                temp_r = 255;
            }
            pixel.blue = static_cast<uint8_t>(temp_b);
            pixel.green = static_cast<uint8_t>(temp_g);
            pixel.red = static_cast<uint8_t>(temp_r);
        }
    }

    





    std::ofstream outFile(outputFilename, std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "Error creating output file: " << outputFilename << std::endl;
        return 1;
    }

    outFile.write(reinterpret_cast<char*>(&header), sizeof(header));
    outFile.write(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(Pixel));

    outFile.close();

    std::cout << "Color constancy applied successfully to " << filename << " and saved as " << outputFilename << std::endl;

    return 0;
}
