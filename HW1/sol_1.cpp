#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

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
    
    if(argc<2){
        std::cerr << "[Error] Need a input -> {input#.bmp}" << std::endl;
        return 1;
    }
    std::string inFileName = argv[1];
    std::string outFileName = "output";
    if(inFileName.find("input")==-1){
        std::cerr << "[Error] Wrong input naming -> {input#.bmp}" << std::endl;
        return 1;
    }
    if(inFileName.find(".bmp")==-1){
        std::cerr << "[Error] Wrong input format: Need .bmp file -> {input#.bmp}" << std::endl;
        return 1;
    }
    int order = inFileName.find(".bmp",0);
    int input_order = inFileName.find("input",0);
    outFileName.append(inFileName, input_order+5,(order-input_order-5)); //

    outFileName += "_flip.bmp";
    std::cout << "[INFO] Input File: " << inFileName << std::endl;
    std::cout << "[INFO] Output File: " << outFileName << std::endl;


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

    //* Flip the image vertically
    const int bytesPerRow = header.width * channel;
    std::vector<char> flippedImageData(imageSize);
    for (int y = 0; y < header.height; y++) {
        for (int x = 0; x < header.width; x++){
            memcpy( &flippedImageData[y * bytesPerRow+ channel*x],
                    &imageData[y* bytesPerRow+ channel*(header.width-x)],
                    channel);
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
    outputFile.write(flippedImageData.data(), imageSize);
    outputFile.close();

    std::cout << "[INFO] Image flipped and saved as " << outFileName << std::endl;

    return 0;
}