//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_STATE_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_STATE_H
#include "context.h"
#include "cs_core_types.h"

struct ContextSensitiveState {
  using SymVarMap = std::map<Symbol *, std::unique_ptr<Variable>>;
  using ContextVars = std::map<Context *, SymVarMap>;
  ContextVars vars_with_ctx;
  std::set<std::unique_ptr<Object>> objects; // only used to keep objects alive
};

#endif // POINTANALYSIS_CONTEXT_SENSITIVE_STATE_H
