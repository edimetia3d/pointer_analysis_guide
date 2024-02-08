//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_INSENSITIVE_STATE_H
#define POINTANALYSIS_CONTEXT_INSENSITIVE_STATE_H

#include <map>
#include <memory>
#include <set>

#include "core_types.h"

struct State {
  std::map<Symbol *, std::unique_ptr<Variable>> vars;
  std::set<std::unique_ptr<Object>> objects; // only used to keep objects alive
};

#endif // POINTANALYSIS_CONTEXT_INSENSITIVE_STATE_H
