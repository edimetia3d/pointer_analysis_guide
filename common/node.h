//
// License: MIT
//

#ifndef POINTANALYSIS_COMMON_NODE_H
#define POINTANALYSIS_COMMON_NODE_H

#include <set>

#include "symbol.h"

struct Object;
using PointeeSet = std::set<Object *>;

struct Node {
  explicit Node(const std::string &identifier)
      : identifier(Symbol::Intern(identifier)) {}

  Symbol *identifier = nullptr;
  PointeeSet pts;
  virtual ~Node() = default; // to enable dynamic_cast
};

#endif // POINTANALYSIS_COMMON_NODE_H
