#pragma once
#include <string>

void check(int ret_value, const std::string &msg)
{
  if (ret_value < 0) {
    std::cout << "Return value(<0), detail: " << msg << std::endl;
    exit(-1);
  }
}

void check_nullptr(void *ptr, cosnt std::string &msg) 
{
  if (ptr == nullptr) {
    std::cout << "Null pointer, detail: " << msg << std::endl;
    exit(-1);
  }
}
