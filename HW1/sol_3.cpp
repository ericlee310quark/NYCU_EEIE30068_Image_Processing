#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string.h>
#include <string>
#include <cstdint>
#include <sstream>

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


char bilinear_imp(double p1, double p2, double p3, double p4, double dx, double dy) {
    int pixel = p1 * (1 - dx) * (1 - dy) + p2 * dx * (1 - dy) + p3 * (1 - dx) * dy + p4 * dx * dy;
    return static_cast<unsigned char>(pixel);
}


int main(int argc, char *argv[]) {
    
    if(argc<2){
        std::cerr << "[Error] Need a input ->  ./sol_3 [input#.bmp] [\"u\" or \"d\"]\n  \"u\": up-sample; \"d\": down-sample" << std::endl;
        return 1;
    }
    //* Quantize Scale Setting
    std::string op_str = argv[2];
    bool is_upsample = 0;

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
    outFileName.append(inFileName, input_order+5,(order-input_order-5)); 
    if(op_str.find("u",0)!=-1){
        outFileName += "_up.bmp";
        is_upsample = 1;
    }
    else if(op_str.find("d",0)!=-1){
        outFileName += "_down.bmp";
        is_upsample = 0;
    }
    

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
    


    //* Calculate new dimensions for upscaling (1.5x)
    const int originalWidth = header.width;
    const int originalHeight = header.height;

    int newWidth, newHeight;
    if(is_upsample){
        newWidth = static_cast<int>(std::round(originalWidth*1.5));
        newHeight = static_cast<int>(std::round(originalHeight*1.5));  
    }
    else{
        newWidth = static_cast<int>(std::round(originalWidth/1.5));
        newHeight = static_cast<int>(std::round(originalHeight/1.5));  
    }
    int new_wid = newWidth;


    //* Deal with padding 0 for (4byte * N)  in one row
    int new_line_byte = newWidth*channel;
    bool pad_4 = 0;
    int res_byte = 0;
    while(((new_line_byte+res_byte)%4)!=0){
        res_byte++;
        pad_4 = 1;
    }
    const int newImageSize = (newWidth) * (newHeight) * (channel)+res_byte*newHeight;
    if (pad_4==1){
        newWidth++;
    }


    std::vector<uint8_t> scaledImageData(newImageSize);

    for (int y = 0; y < newHeight; y++) {
        for (int x = 0; x < newWidth; x++) {
            if((pad_4)&&(x==newWidth-1)){
                for(int res_ind=0;res_ind<res_byte;res_ind++){
                    scaledImageData[(y * newWidth + x) * channel + res_ind] = 0;
                }
                continue;
            }
            
                double x2new, y2new;
                
                if(is_upsample){
                    x2new = double(x) / 1.5;
                    y2new = double(y) / 1.5;
                }
                else{
                    x2new = double(x) * 1.5;
                    y2new = double(y) * 1.5;
                }

                double x1 = (std::floor(x2new));
                double y1 = (std::floor(y2new));
                double x2, y2;
                x2 = x1 + 1;
                y2 = y1 + 1;

                if(int(x1)>=originalWidth){
                    x1 = originalWidth;
                    x2 = originalWidth;
                    std::cout<< "[Warning] OUT of Pixel"<< std::endl;
                }
                else    x2 = x1 + 1;
                if(int(y1)>=originalHeight){
                    y1 = originalHeight;
                    y2 = originalHeight;
                    std::cout<< "[Warning] OUT of Pixel"<< std::endl;
                }
                else    y2 = y1 + 1;

                double dx = x2new - x1;
                double dy = y2new - y1;


                for (int ch = 0; ch < channel; ch++) {

                    unsigned char p1 = imageData[(y1 * originalWidth + x1) * channel + ch];
                    unsigned char p2 = imageData[(y1 * originalWidth + x2) * channel + ch];
                    unsigned char p3 = imageData[(y2 * originalWidth + x1) * channel + ch];
                    unsigned char p4 = imageData[(y2 * originalWidth + x2) * channel + ch];

                    scaledImageData[(y * newWidth + x) * channel + ch] =
                        static_cast<unsigned char>((bilinear_imp(double(p1), double(p2), double(p3), double(p4), double(dx), double(dy))));       
                }
            
        }
    }
    
    BMPHeader header1;
    header1.type =          header.type;
    header1.size =         54 + newImageSize;
    header1.reserved =     0;
    header1.offset =        header.offset;
    header1.dibHeaderSize = header.dibHeaderSize;
    header1.width =         new_wid;
    header1.height =        newHeight;
    header1.planes =        header.planes;
    header1.bitsPerPixel =  header.bitsPerPixel;
    header1.compression =   header.compression;
    header1.imageSize =     newImageSize;
    header1.xPixelsPerMeter = header.xPixelsPerMeter;
    header1.yPixelsPerMeter = header.yPixelsPerMeter;
    header1.colorsUsed      = header.colorsUsed;
    header1.colorsImportant = header.colorsImportant;



    //header.size = header.size - imageSize + header.imageSize;
    std::cout<<"[INFO] New width: "<<header1.width<<std::endl;
    std::cout<<"[INFO] New height: "<<header1.height<<std::endl;
    //std::cout<<"new imageSize: "<<header.imageSize<<std::endl;

    //* Create/Open the output bmp file
    std::ofstream outputFile(outFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "[Error] Failed to create output BMP file." << std::endl;
        return 1;
    }

    // Write the BMP header and scaled image data
    std::cout<<"[INFO] sizeof(BMPHeader): "<<sizeof(BMPHeader)<<std::endl;
    std::cout<<"[INFO] outputImageSize: "<<header1.imageSize<<std::endl;
    std::cout<<"[INFO] offset: "<<header1.offset<<std::endl;
    std::cout<<"[INFO] dibHeaderSize: "<<header1.dibHeaderSize<<std::endl;
    std::cout<<"[INFO] Size: "<<header1.size<<std::endl;
    

    //* Output the header and image raw data
    outputFile.write(reinterpret_cast<const char*>(&header1), sizeof(BMPHeader));
    outputFile.write((const char *)(scaledImageData.data()), newImageSize);
    outputFile.close();

    std::cout << "[INFO] Image flipped and saved as " << outFileName << std::endl;
  


    return 0;
}