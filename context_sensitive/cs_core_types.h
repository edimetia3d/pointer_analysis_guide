//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_CORE_TYPES_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_CORE_TYPES_H

#include <map>
#include <memory>
#include <set>
#include <string>

#include "../common/node.h"
#include "../common/object_base.h"
#include "../common/symbol.h"
#include "context.h"

struct Variable : public Node {
  Variable(const std::string &identifier, Context *context)
      : Node(identifier), context(context) {}
  Context *context;
};

#endif // POINTANALYSIS_CONTEXT_SENSITIVE_CORE_TYPES_H
