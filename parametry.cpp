#include "parametry.h"

#include <cstdlib>

std::string intToString(int i) {
      char c[10];
      itoa(i, c, 10);
      int j = 0;
      std::string str;
      while(c[j] != '\0') {
            str += c[j];
            j++;
      }
      return str;
}
