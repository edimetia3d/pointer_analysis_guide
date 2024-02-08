//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_CONTEXT_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_CONTEXT_H

#include <vector>

#include "../common/ir.h"

/**
 * In this design, context only stores the call_chain it related to. It is just
 * a unique identifier to help getting context related information.
 */

struct Context {
  static Context *Intern(std::vector<const Inst *> call_chain);
  std::vector<const Inst *> call_chain;
};

#endif // POINTANALYSIS_CONTEXT_SENSITIVE_CONTEXT_H
