//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_PFG_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_PFG_H

#include <map>
#include <set>

#include "../common/pfg_base.h"
#include "cs_state.h"

struct ContextSensitivePFG : public PFGBase<ContextSensitiveState> {

  Variable *GetVariable(const std::string &identifier, Context *ctx) {
    auto sym = Symbol::Intern(identifier);
    auto &sym_map = state.vars_with_ctx[ctx];
    if (sym_map.find(sym) == sym_map.end()) {
      sym_map[sym] = std::make_unique<Variable>(identifier, ctx);
    }
    return sym_map[sym].get();
  }
};

#endif // POINTANALYSIS_CONTEXT_SENSITIVE_PFG_H
