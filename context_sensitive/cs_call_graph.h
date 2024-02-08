//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_CALL_GRAPH_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_CALL_GRAPH_H
#include <map>

#include "../common/ir.h"
#include "context.h"

// means a call_inst that will calls callee at a specific context
struct CallPair {
  Inst *call_inst;
  const Function *callee;
  Context *context;
  bool operator<(const CallPair &rhs) const {
    if (call_inst != rhs.call_inst)
      return call_inst < rhs.call_inst;
    if (callee != rhs.callee)
      return callee < rhs.callee;
    return context < rhs.context;
  }
};

struct CallGraph {
  bool HasEdge(const Function *caller, Inst *call_inst, const Function *callee,
               Context *context) {
    return call_map[caller].count({call_inst, callee, context});
  }
  void AddEdge(const Function *caller, Inst *call_inst, const Function *callee,
               Context *context) {
    call_map[caller].insert({call_inst, callee, context});
  }
  std::map<const Function *, std::set<CallPair>>
      call_map; // call_map[caller] stores inst in caller that calls callee
};
#endif // POINTANALYSIS_CONTEXT_SENSITIVE_CALL_GRAPH_H
