//
// License: MIT
//

#ifndef POINTANALYSIS_COMMON_SYMBOL_H
#define POINTANALYSIS_COMMON_SYMBOL_H

#include <string>

struct Symbol {
  static Symbol *Intern(const std::string &value);
  std::string value;

private:
  Symbol();
};

#endif // POINTANALYSIS_COMMON_SYMBOL_H
