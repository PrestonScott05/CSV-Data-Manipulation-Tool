#include "Utility.h"
#include <sstream>

bool isValidDouble(std::string value) {
    std::istringstream ss(value);
    double val;
    ss >> val;
    return ss.eof() && !ss.fail();
}
