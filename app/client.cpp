#include "common.h"
#include "stb_image.h"
#include <iostream>

int main(int argc, char **argv) {
  Shmemaphore shma4("ibis");

  for (int i = 0; i < 10; i++) {
    // Ð?i l?nh t? server
    shma4.waitForRequest();

    // Nh?n kích thu?c d? li?u t? server
    uint64_t dataSize = 0;
    dataSize = *((uint64_t *)shma4.getHeader(sizeof(uint64_t)));

    // Nh?n chu?i l?nh t? server
    const char *rawData = (const char *)shma4.getData(dataSize);
    std::string command(rawData, dataSize);

    std::cout << "Server wants " << command << std::endl;

    // T?o tên t?p ?nh
    std::string fname = "img/" + command + ".jpg";

    // T?i ?nh
    int w, h, c;
    unsigned char *image = stbi_load(fname.c_str(), &w, &h, &c, 0);
    if (!image) {
        std::cerr << "Failed to load image: " << fname << std::endl;
        continue; // N?u không t?i du?c ?nh, b? qua vòng l?p này
    }

    // Tính toán kích thu?c c?a d? li?u hình ?nh
    size_t imageBytes = w * h * c * sizeof(unsigned char);
    if (imageBytes <= 0) {
        std::cerr << "Invalid image size: " << imageBytes << std::endl;
        stbi_image_free(image);
        continue;
    }

    // T?o header ch?a thông tin v? kích thu?c và chi?u ?nh
    std::vector<uint64_t> imageDims = {imageBytes, (uint64_t)w, (uint64_t)h, (uint64_t)c};
    std::cout << "I'm sending " << imageBytes << " bytes" << std::endl;

    // G?i header ch?a kích thu?c và thông tin ?nh
    shma4.setHeader((void *)imageDims.data(), imageDims.size() * sizeof(uint64_t));

    // G?i d? li?u ?nh
    shma4.setData(image, imageBytes);

    // Gi?i phóng b? nh? ?nh sau khi dã g?i xong
    stbi_image_free(image);

    // Ðánh d?u vi?c g?i hoàn thành
    shma4.responseComplete();
  }

  return 0;
}

