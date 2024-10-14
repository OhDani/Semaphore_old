#include "common.h"
#include "stb_image.h"
#include <iostream>

int main(int argc, char **argv) {
  Shmemaphore shma4("ibis");

  for (int i = 0; i < 10; i++) {
    // �?i l?nh t? server
    shma4.waitForRequest();

    // Nh?n k�ch thu?c d? li?u t? server
    uint64_t dataSize = 0;
    dataSize = *((uint64_t *)shma4.getHeader(sizeof(uint64_t)));

    // Nh?n chu?i l?nh t? server
    const char *rawData = (const char *)shma4.getData(dataSize);
    std::string command(rawData, dataSize);

    std::cout << "Server wants " << command << std::endl;

    // T?o t�n t?p ?nh
    std::string fname = "img/" + command + ".jpg";

    // T?i ?nh
    int w, h, c;
    unsigned char *image = stbi_load(fname.c_str(), &w, &h, &c, 0);
    if (!image) {
        std::cerr << "Failed to load image: " << fname << std::endl;
        continue; // N?u kh�ng t?i du?c ?nh, b? qua v�ng l?p n�y
    }

    // T�nh to�n k�ch thu?c c?a d? li?u h�nh ?nh
    size_t imageBytes = w * h * c * sizeof(unsigned char);
    if (imageBytes <= 0) {
        std::cerr << "Invalid image size: " << imageBytes << std::endl;
        stbi_image_free(image);
        continue;
    }

    // T?o header ch?a th�ng tin v? k�ch thu?c v� chi?u ?nh
    std::vector<uint64_t> imageDims = {imageBytes, (uint64_t)w, (uint64_t)h, (uint64_t)c};
    std::cout << "I'm sending " << imageBytes << " bytes" << std::endl;

    // G?i header ch?a k�ch thu?c v� th�ng tin ?nh
    shma4.setHeader((void *)imageDims.data(), imageDims.size() * sizeof(uint64_t));

    // G?i d? li?u ?nh
    shma4.setData(image, imageBytes);

    // Gi?i ph�ng b? nh? ?nh sau khi d� g?i xong
    stbi_image_free(image);

    // ��nh d?u vi?c g?i ho�n th�nh
    shma4.responseComplete();
  }

  return 0;
}

