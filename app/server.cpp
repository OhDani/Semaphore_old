#include <errno.h>
#include <random>
#include "common.h"
#include "stb_image_write.h"

int main(int argc, char **argv) {
  Shmemaphore shma4("ibis", true);

  // command to send to client (loads an image)
  std::vector<std::string> commands = {"apple", "ball", "car", "dog"};
  std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<size_t> rng{0, commands.size() - 1};

  for (int i = 0; i < 10; i++) {
    std::string command = commands[rng(gen)];
    std::cout << "I want " << command << std::endl;

    uint64_t dataSize = command.size();
    shma4.setHeader(&dataSize, sizeof(dataSize));
    shma4.setData(command.c_str(), dataSize);

    shma4.requestComplete();
    shma4.waitForResponse();
  }

  return 0;
}