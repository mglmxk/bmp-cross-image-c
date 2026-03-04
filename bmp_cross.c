#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


struct BmpHeader {
    uint16_t id;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset_of_pixels;
};
struct DibHeader {
    uint32_t headerSize;
    uint32_t width;
    uint32_t height;
};
struct Pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};


struct BmpHeader read_bmp_header(FILE* file) {
    struct BmpHeader header;
    char buffer[4];

    for (size_t j = 0; j < 2; j++) 
    {
        buffer[j] = fgetc(file);
    }
    header.id = *(uint16_t*)buffer;

    for (size_t i = 0; i < 4; i++) 
    {
        buffer[i] = fgetc(file);
    }
    header.fileSize = *(uint32_t*)buffer;
    for (size_t i = 0; i < 4; i++) 
    {
        fgetc(file);
    }

    for (size_t i = 0; i < 4; i++) 
    {
        buffer[i] = fgetc(file);
    }
    header.offset_of_pixels = *(uint32_t*)buffer;

    return header;
}

struct DibHeader read_dib_header(FILE* file) {
    struct DibHeader header;
    char buffer[4];
    for (size_t i = 0; i < 4; i++) 
    {
        buffer[i] = fgetc(file);
    }
    header.headerSize = *(uint32_t*)buffer;
    for (size_t i = 0; i < 4; i++) 
    {
        buffer[i] = fgetc(file);
    }
    header.width = *(uint32_t*)buffer;
    for (size_t i = 0; i < 4; i++) 
    {
        buffer[i] = fgetc(file);
    }
    header.height = *(uint32_t*)buffer;
    return header;
}

void drawLineRTL(struct Pixel** pixels, int width, struct Pixel color) {
    for (int x = 0; x < width; x++) 
    {
        int y = x;
        if (y < width) {
            pixels[x][y] = color;
        }
        else {
            printf("y is out of range\n");
        }
    }
}
void drawLineLTR(struct Pixel** pixels, int width, struct Pixel color) {
    for (int x = 0; x < width; x++) 
    {
        int y = width - 1 - x;
        if (y < width) {
            pixels[x][y] = color;
        }
        else {
            printf("y is out of range\n");
        }
    }
}

int main() {

    FILE* file = fopen("1.bmp", "rb");

    struct BmpHeader header = read_bmp_header(file);
    struct DibHeader dib = read_dib_header(file);

    uint32_t pixelsCount = dib.width * dib.height;

    struct Pixel** pixels = (struct Pixel**)malloc(sizeof(struct Pixel*) * dib.width);

    for (size_t i = 0; i < dib.width; i++) {
        pixels[i] = (struct Pixel*)malloc(sizeof(struct Pixel) * dib.height);
    }

    fpos_t position;
    fgetpos(file, &position);

    while ((uint32_t)position < header.offset_of_pixels) {
        fgetc(file);
        position++;
    }

    for (uint32_t i = 0; i < dib.height; i++) {
        for (uint32_t j = 0; j < dib.width; j++) {
            struct Pixel pixel;
            pixel.blue = fgetc(file);
            pixel.green = fgetc(file);
            pixel.red = fgetc(file);
            pixels[j][i] = pixel;
        }
    }
    fclose(file);

    struct Pixel color;
    color.red = 0;
    color.green = 0;
    color.blue = 255;

    drawLineRTL(pixels, dib.width, color);
    drawLineLTR(pixels, dib.width, color);
    FILE* source = fopen("1.bmp", "rb");
    FILE* dest = fopen("2.bmp", "wb");
    for (uint32_t i = 0; i < header.offset_of_pixels; i++) { 
        char c = fgetc(source);
        fputc(c, dest);
    }
    for (uint32_t i = 0; i < dib.height; i++) {
        for (uint32_t j = 0; j < dib.width; j++) {
            fputc(pixels[j][i].blue, dest);
            fputc(pixels[j][i].green, dest);
            fputc(pixels[j][i].red, dest);
        }
    }


    
    fclose(dest);
    for (uint32_t i = 0; i < dib.width; i++)
    {
        free(pixels[i]);
    }
    free(pixels);
    return 0;
}