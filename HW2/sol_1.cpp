#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#pragma pack(1) // Pack struct members tightly
typedef struct {
    uint16_t type;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitCount;
    uint32_t compression;
    uint32_t sizeImage;
    int32_t xPelsPerMeter;
    int32_t yPelsPerMeter;
    uint32_t clrUsed;
    uint32_t clrImportant;
} BMPInfoHeader;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} RGB;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s input.bmp output.bmp gamma\n", argv[0]);
        return 1;
    }

    const char* inputFileName = argv[1];
    const char* outputFileName = argv[2];
    double gamma = atof(argv[3]);

    FILE *inputFile = fopen(inputFileName, "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    BMPHeader header;
    fread(&header, sizeof(header), 1, inputFile);

    if (header.type != 0x4D42) {
        fclose(inputFile);
        printf("Not a BMP file.\n");
        return 1;
    }

    BMPInfoHeader infoHeader;
    fread(&infoHeader, sizeof(infoHeader), 1, inputFile);

    if (infoHeader.bitCount != 24) {
        fclose(inputFile);
        printf("Only 24-bit BMPs are supported.\n");
        return 1;
    }

    // Calculate the gamma correction table
    uint8_t gammaTable[256];
    for (int i = 0; i < 256; i++) {
        gammaTable[i] = (uint8_t)(pow(i / 255.0, 1.0 / gamma) * 255.0);
    }

    // Create the output BMP file
    FILE* outputFile = fopen(outputFileName, "wb");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    fwrite(&header, sizeof(header), 1, outputFile);
    fwrite(&infoHeader, sizeof(infoHeader), 1, outputFile);

    // Process each pixel
    RGB pixel;
    for (int y = 0; y < infoHeader.height; y++) {
        for (int x = 0; x < infoHeader.width; x++) {
            fread(&pixel, sizeof(pixel), 1, inputFile);

            pixel.red = gammaTable[pixel.red];
            pixel.green = gammaTable[pixel.green];
            pixel.blue = gammaTable[pixel.blue];

            fwrite(&pixel, sizeof(pixel), 1, outputFile);
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    printf("Gamma correction completed.\n");

    return 0;
}
