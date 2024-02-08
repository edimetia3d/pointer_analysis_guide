//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_H
#define POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_H
// -------------------------------- PFG --------------------------------
#include <map>
#include <set>

#include "../common/pfg_base.h"
#include "state.h"

/**
 * PFG will be used to represent the Program Flow Graph and a helper to update
 * the state.
 */
struct PFG : public PFGBase<State> {
  using PFGBase::PFGBase;

  Variable *GetVariable(const std::string &identifier) {
    auto sym = Symbol::Intern(identifier);
    if (state.vars.find(sym) == state.vars.end()) {
      state.vars[sym] = std::make_unique<Variable>(identifier);
    }
    return state.vars[sym].get();
  }
};
#endif // POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_H
