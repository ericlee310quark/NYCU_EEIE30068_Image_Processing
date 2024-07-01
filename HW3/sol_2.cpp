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
// Clamp function to ensure pixel values stay within [0, 255]
uint8_t clamp(double value) {
    return static_cast<uint8_t>(std::min(std::max(value, 0.0), 255.0));
}

// Convert RGB to HSL
void RGBtoHSL(double r, double g, double b, double& h, double& s, double& l) {
    double cMax = std::max({r, g, b});
    double cMin = std::min({r, g, b});
    double delta = cMax - cMin;

    // Hue calculation
    if (delta == 0)
        h = 0;
    else if (cMax == r && (g>=b))
        //h = 60 * fmod(((g - b) / delta), 6);
        h = 60 * ((g - b) / delta);
    else if(cMax == r && (g<b))
        h = 60 * ((g - b) / delta)+360;
    else if (cMax == g)
        h = 60 * (((b - r) / delta))+120;
    else if (cMax == b)
        h = 60 * (((r - g) / delta))+240;

    // Lightness calculation
    l = (cMax + cMin) / 2;
    // Saturation calculation
    if (cMax == 0)
        s = 0;
    else if (l>0.5)
        if(delta<0.001){
            s=0;
        }
        else
            s = delta / (2-2*l);
    else
        s = delta / (2*l);


}

// Convert HSL to RGB
void HSLtoRGB(double h, double s, double l, double& r, double& g, double& b) {
    double c = (1 - fabs(2 * l - 1)) * s;
    double x = c * (1 - fabs(fmod(h / 60, 2) - 1));
    double m = l - c / 2;

    if (h >= 0 && h < 60) {
        r = c;
        g = x;
        b = 0;
    } else if (h >= 60 && h < 120) {
        r = x;
        g = c;
        b = 0;
    } else if (h >= 120 && h < 180) {
        r = 0;
        g = c;
        b = x;
    } else if (h >= 180 && h < 240) {
        r = 0;
        g = x;
        b = c;
    } else if (h >= 240 && h < 300) {
        r = x;
        g = 0;
        b = c;
    } else if (h >= 300 && h < 360) {
        r = c;
        g = 0;
        b = x;
    }

    r = (r + m) * 255;
    g = (g + m) * 255;
    b = (b + m) * 255;


}
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
    int channel = header.bitsPerPixel/8;
    const int imageSize = header.width * header.height * channel;
    file.seekg(header.dataOffset, std::ios::beg);
    file.read(reinterpret_cast<char*>(pixels.data()), pixels.size() * sizeof(Pixel));
    std::vector<char> imageData(imageSize);
    int cnt =0;
    for(auto& pixel : pixels){
        imageData[cnt] = pixel.blue;
        imageData[cnt+1] = pixel.blue;
        imageData[cnt+2] = pixel.blue;
        cnt+=3;
    }



    file.close();



    if(alg_type.find("1")!=-1){
        // Adjust saturation
        double saturationFactor = 0; // You can adjust this value as needed
        // Transform into HSL, strengthen S, L
        for (auto& pixel : pixels) {
            double r = pixel.red / 255.0;
            double g = pixel.green / 255.0;
            double b = pixel.blue / 255.0;

            double h, s, l;
            RGBtoHSL(r, g, b, h, s, l);
            double contrastFactor = 0.5;
            // Adjust saturation
            s += (s*1);
            l += (l*0.1);
            
            s = std::min(std::max(s, 0.0), 1.0);
            l = std::min(std::max(l, 0.0), 1.0);               
            // Transform back to RGB
            HSLtoRGB(h, s, l, r, g, b);
            pixel.red = clamp(r);
            pixel.green = clamp(g);
            pixel.blue = clamp(b);
    }
    
    uint8_t gammaTable[256];
    double gamma = 1.2;
    // adjust gamma ratio with RGB
    // and adjust R, B channel 
    for (int i = 0; i < 256; i++) {
        gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
    }
    for (auto& pixel : pixels) {
        pixel.red = gammaTable[pixel.red]*0.75;
        pixel.green = gammaTable[pixel.green]*0.87;
        pixel.blue = gammaTable[pixel.blue];//*0.95;
        pixel.red = clamp(pixel.red);
        pixel.green = clamp(pixel.green);
        pixel.blue = clamp(pixel.blue);
    }
    
 

    }
    else if(alg_type.find("2")!=-1){
       

        double saturationFactor = 0; // You can adjust this value as needed
        //Transform into HSL and make blue more deeper and obvious
        //Strengthen S, L 
        for (auto& pixel : pixels) {
            double r = pixel.red / 255.0;
            double g = pixel.green / 255.0;
            double b = pixel.blue / 255.0;

            double h, s, l;
            RGBtoHSL(r, g, b, h, s, l);
            double contrastFactor = 1;
            // Adjust saturation
            s= pow(s / 1.0, 1.0 / 3);
            if(h>130&&(h<250)){
                h = pow(h / 360, 1.0 / (1+0.18*((h-130)/(225-130))))*360;
            }

            l += (l*0.05);
            h = std::min(std::max(h, 0.0), 360.0);
            s = std::min(std::max(s, 0.0), 1.0);
            l = std::min(std::max(l, 0.0), 1.0);
            
            s = (s-0.5)*contrastFactor +0.5;//s*1.1;//(pow(s / 1.0, 1.0 / 0.5) * 1.0);
            s = std::min(std::max(s, 0.0), 1.0);     

            //Translate back to RGB and do constrast      
            HSLtoRGB(h, s, l, r, g, b);

            r = (r-127)*1.1 +127;
            g = (g-127)*1.1 +127;
            b = (b-127)*1.1 +127;
            //use linear superposition to sum up two picture
            pixel.red = clamp((uint8_t)(pixel.red*0.5 +  clamp(r)*0.5)*1);
            pixel.green = clamp((uint8_t)(pixel.green*0.5 + clamp(g)* 0.5)*0.9);
            pixel.blue = clamp((uint8_t)(pixel.blue*0.5 + clamp(b)* 0.5)*0.9);

    }   



    }
    else if(alg_type.find("3")!=-1){
        // adjust gamma ratio with RGB
        uint8_t gammaTable[256];
        double gamma = 1.7;
        for (int i = 0; i < 256; i++) {
            gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
        }
        for (auto& pixel : pixels) {
            pixel.red = gammaTable[pixel.red];
            pixel.green = gammaTable[pixel.green];
            pixel.blue = gammaTable[pixel.blue];

        }
    }
    else if(alg_type.find("4")!=-1){
        
        // adjust gamma ratio with RGB
        uint8_t gammaTable[256];
        double gamma = 0.75;
        for (int i = 0; i < 256; i++) {
            gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
        }
        double r_c =0.0, g_c =0.0, b_c = 0.0;

        double cons_thre_r=127, cons_thre_g=127, cons_thre_b = 45;
        for (auto& pixel : pixels) {

            if((gammaTable[pixel.blue]*0.8-(gammaTable[pixel.green]+gammaTable[pixel.red])/3)>10)
                b_c =0.85;
            else
                b_c = 1;

            pixel.red = clamp((gammaTable[int(pixel.red*0.8)]));
            pixel.green = clamp((gammaTable[int(pixel.green*0.8)]));
            pixel.blue = clamp((gammaTable[int(pixel.blue*0.6)]-cons_thre_b)*b_c+(cons_thre_b));

        }


        //amplify the saturation and lower L
        for (auto& pixel : pixels) {
            double r = pixel.red / 255.0;
            double g = pixel.green / 255.0;
            double b = pixel.blue / 255.0;

            double h, s, l;
            RGBtoHSL(r, g, b, h, s, l);
            s= pow(s / 1.0, 1.0 / 1.65);
            l = pow(l / 1.0, 0.82);
            HSLtoRGB(h, s, l, r, g, b);
            pixel.red =r; //clamp((uint8_t)(pixel.red*0.75 +  clamp(r)));
            pixel.green = g;//clamp((uint8_t)(pixel.green*0.75 + clamp(g)));
            pixel.blue = b;//clamp((uint8_t)(pixel.blue*0.75 + clamp(b)));

    }   

    //do MaxRGB again
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
        double scaleB = avgG / avgB*0.9;
        for (auto& pixel : pixels) {
            pixel.red = static_cast<uint8_t>(std::min(255.0, pixel.red * scaleR));
            pixel.blue = static_cast<uint8_t>(std::min(255.0, pixel.blue * scaleB));
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
