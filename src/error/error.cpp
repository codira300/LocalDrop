#include "error.h"
#include <iostream>
#include <cstdlib>

void ErrorLog(const std::string& err){
    std::cout << err << std::endl;
    exit(1);
}