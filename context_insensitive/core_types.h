//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_INSENSITIVE_CORE_TYPES_H
#define POINTANALYSIS_CONTEXT_INSENSITIVE_CORE_TYPES_H
#include <map>
#include <memory>
#include <set>

#include "../common/node.h"
#include "../common/object_base.h"
#include "../common/symbol.h"

struct Variable : public Node {
  using Node::Node;
};

#endif // POINTANALYSIS_CONTEXT_INSENSITIVE_CORE_TYPES_H
