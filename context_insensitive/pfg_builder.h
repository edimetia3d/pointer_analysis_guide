//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_BUILDER_H
#define POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_BUILDER_H
#include <cassert>

#include <set>
#include <vector>

#include "../common/ir.h"
#include "../common/pfg_builder_base.h"
#include "pfg.h"

struct PFGBuilder : public PFGBuilderBase<PFG> {
  PFGBuilder(PFG *pfg, const Function *fn) : PFGBuilderBase<PFG>(pfg), fn(fn) {
    // Populate param with all possible objects
    for (int i = 0; i < fn->params.size(); i++) {
      auto param_node = pfg->GetVariable(fn->params[i]);
      auto param_t = fn->param_types[i];
      // we can walk through the inheritance tree to find all possible objects
      // types and create a new object for each type, then populate into the
      // `param_node`
    }
    ReachToFn(fn);
  }

  void Solve() {
    if (!pfg) {
      return; // already solved
    }
    while (!wl.empty()) {
      // remove one item
      auto outdated = wl.back();
      wl.pop_back();
      auto target = outdated.node;
      auto real_new = SetDiff(outdated.new_pointees, target->pts);
      Propagate(target, real_new);
      auto var_target = dynamic_cast<Variable *>(target);
      if (!var_target) {
        continue;
      }

      // when target is a variable, eg,  "x" then all x related instruction will
      // be affected for x.f = y, we need to add new edge from y to x.f for each
      // new obj of x for y = x.f, we need to add new edge from x.f to y for
      // each new obj of x We ignore x() and x.foo() for now , since we can not
      // infer the possible result object we can only infer any type of object
      // can be returned, like what we do for the param
      for (auto new_obj : real_new) {
        ProcessSetAttr(var_target, new_obj);
        ProcessGetAttr(var_target, new_obj);
      }
    }
    pfg = nullptr; // mark as solved
  }

  void ReachToFn(const Function *func) {
    for (auto inst : GetAllNewObj(func->insts)) {
      // dst = new T()
      auto new_obj = pfg->GetNewObject(inst->type);
      auto dst = pfg->GetVariable(inst->result_var);
      AddEdge(new_obj, dst);
    }
    for (auto inst : GetAllAssign(func->insts)) {
      // dst = src
      auto dst = pfg->GetVariable(inst->result_var);
      auto src = pfg->GetVariable(inst->value_var);
      AddEdge(src, dst);
    }
  }

  void ProcessGetAttr(const Variable *target, Object *new_obj) {
    for (auto get_attr_inst :
         GetAllGetAttrOnVar(fn->insts, target->identifier->value)) {
      assert(target->identifier->value == get_attr_inst->base_var);
      auto x_f = new_obj->GetFiled(get_attr_inst->field);
      auto y = pfg->GetVariable(get_attr_inst->result_var);
      AddEdge(x_f, y);
    }
  }

  void ProcessSetAttr(const Variable *target, Object *new_obj) {
    for (auto set_attr_inst :
         GetAllSetAttrOnVar(fn->insts, target->identifier->value)) {
      assert(target->identifier->value == set_attr_inst->base_var);
      auto x_f = new_obj->GetFiled(set_attr_inst->field);
      auto y = pfg->GetVariable(set_attr_inst->value_var);
      AddEdge(y, x_f);
    }
  }

private:
  const Function *fn;
};
#endif // POINTANALYSIS_CONTEXT_INSENSITIVE_PFG_BUILDER_H
