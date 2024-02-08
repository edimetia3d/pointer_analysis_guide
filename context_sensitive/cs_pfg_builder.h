//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_SENSITIVE_CS_PFG_BUILDER_H
#define POINTANALYSIS_CONTEXT_SENSITIVE_CS_PFG_BUILDER_H
#include <cassert>

#include <set>
#include <vector>

#include "../common/ir.h"
#include "../common/pfg_builder_base.h"
#include "cs_call_graph.h"
#include "cs_pfg.h"

std::set<CallInst *> GetAllStaticCall(
    const std::set<Inst *>
        &all); // return all call that can be resolved at compile time
std::set<CallMethod *> GetAllDynamicCallMethod(const std::set<Inst *> &all);
std::set<CallInst *> GetAllDynamicCall(const std::set<Inst *> &all);
std::set<RetInst *> GetAllRet(const std::set<Inst *> &all);
bool IsMethod(const Function *fn);

// When k_cfa = 0, it will be context-insensitive
struct PFGCGBuilder : public PFGBuilderBase<ContextSensitivePFG> {

  PFGCGBuilder(ContextSensitivePFG *pfg, CallGraph *cg,
               const Function *call_root, const Module *module, int k_cfa = 3)
      : PFGBuilderBase<ContextSensitivePFG>(pfg), cg(cg), module_(module),
        k_cfa(k_cfa) {
    // We assume the call_root has no param , so we dont need to populate the
    // params with all possible objects
    ReachToFn(call_root, Context::Intern({}));
  }

  void Solve() {
    if (!pfg) {
      return; // already solved
    }

    // Solve PFG
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
      // each new obj of x for callable `x()`, new function may be called when x
      // point to a new obj for `x.foo()`, new method may be called when x point
      // to a new obj
      for (auto new_obj : real_new) {
        ProcessSetAttr(var_target, new_obj);
        ProcessGetAttr(var_target, new_obj);
        ProcessCall(var_target, new_obj);
        ProcessCallMethod(var_target, new_obj);
      }
    }
    pfg = nullptr; // mark as solved
  }
  void ReachToFn(const Function *fn, Context *ctx) {
    // Reach a function will handle all general instructions in it
    if (context_to_fn.find(ctx) != context_to_fn.end()) {
      assert(context_to_fn[ctx] == fn);
      return;
    }
    context_to_fn[ctx] = fn;

    for (auto inst : GetAllNewObj(fn->insts)) {
      auto new_obj = pfg->GetNewObject(inst->type);
      auto dst = pfg->GetVariable(inst->result_var, ctx);
      AddEdge(new_obj, dst);
    }
    for (auto inst : GetAllAssign(fn->insts)) {
      auto dst = pfg->GetVariable(inst->result_var, ctx);
      auto src = pfg->GetVariable(inst->value_var, ctx);
      AddEdge(src, dst);
    }
    for (auto call_inst : GetAllStaticCall(fn->insts)) {
      auto callee_fn = module_->Lookup(call_inst->callee);
      std::vector<std::string> args = call_inst->args;
      std::string result_var = call_inst->result_var;
      AddNewCallInCG(fn, ctx, call_inst, callee_fn, args, result_var, nullptr);
    }
  }

  void AddNewCallInCG(const Function *caller, Context *caller_ctx,
                      Inst *call_inst, const Function *callee,
                      std::vector<std::string> args,
                      const std::string &result_var, Object *receiver) {

    auto GetCalleeContext = [](Context *caller_context, const Inst *call_inst,
                               int k) {
      auto call_chain = caller_context->call_chain;
      call_chain.push_back(call_inst);
      while (call_chain.size() > k) {
        call_chain.erase(call_chain.begin());
      }
      return Context::Intern(call_chain);
    };
    Context *callee_ctx = nullptr;

    if (k_cfa == 0) {
      // when k_cfa = 0, the analysis will be context-insensitive
      // Create a dummy context, so that function will share the same context
      // when get called . This will also help to avoid the name conflict in the
      // context-insensitive analysis
      const Inst *dummy_inst = reinterpret_cast<const Inst *>(callee);
      callee_ctx = Context::Intern({dummy_inst});
    } else {
      callee_ctx = GetCalleeContext(caller_ctx, call_inst, k_cfa);
    }

    // In our implementation, every receiver is a newly found object
    // so always try to add an edge for it
    if (!receiver) {
      // When calling a method, the arg number will be one more than the
      // callee's params, and the first arg will be the receiver
      auto this_var = pfg->GetVariable("this", callee_ctx);
      AddEdge(receiver, this_var);
    }

    if (cg->HasEdge(caller, call_inst, callee, callee_ctx)) {
      // If has edge, args in the caller context already linked to params in
      // callee context
      return;
    }
    cg->AddEdge(caller, call_inst, callee, callee_ctx);
    for (int i = 0; i < callee->params.size(); ++i) {
      auto param = pfg->GetVariable(callee->params[i], callee_ctx);
      auto arg = pfg->GetVariable(args[i], caller_ctx);
      AddEdge(arg, param);
    }
    for (auto ret_inst : GetAllRet(callee->insts)) {
      auto ret_var = pfg->GetVariable(ret_inst->value_var, callee_ctx);
      auto call_ret = pfg->GetVariable(result_var, caller_ctx);
      AddEdge(ret_var, call_ret);
    }
    ReachToFn(callee, callee_ctx);
  }

  void ProcessCallMethod(const Variable *target, Object *new_obj) {
    auto target_parent_fn = context_to_fn[target->context];
    for (auto call_method_inst :
         GetAllDynamicCallMethod(target_parent_fn->insts)) {
      assert(target->identifier->value == call_method_inst->base_var);
      auto callee = new_obj->GetMethod(call_method_inst->method);
      AddNewCallInCG(target_parent_fn, target->context, call_method_inst,
                     callee, call_method_inst->args,
                     call_method_inst->result_var, new_obj);
    }
  }

  void ProcessCall(const Variable *target, Object *new_obj) {
    auto tareget_parent_fn = context_to_fn[target->context];
    for (auto call_inst : GetAllDynamicCall(tareget_parent_fn->insts)) {
      assert(target->identifier->value == call_inst->callee);
      auto callee = module_->Lookup(
          new_obj->identifier
              ->value); // obj itself is a callable, so must be in module
      AddNewCallInCG(tareget_parent_fn, target->context, call_inst, callee,
                     call_inst->args, call_inst->result_var, nullptr);
    }
  }
  void ProcessGetAttr(const Variable *target, Object *new_obj) {
    auto ctx_fn = context_to_fn[target->context];
    for (auto get_attr_inst :
         GetAllGetAttrOnVar(ctx_fn->insts, target->identifier->value)) {
      assert(target->identifier->value == get_attr_inst->base_var);
      auto x_f = new_obj->GetFiled(get_attr_inst->field);
      auto y = pfg->GetVariable(get_attr_inst->result_var, target->context);
      AddEdge(x_f, y);
    }
  }
  void ProcessSetAttr(const Variable *target, Object *new_obj) {
    auto ctx_fn = context_to_fn[target->context];
    for (auto set_attr_inst :
         GetAllSetAttrOnVar(ctx_fn->insts, target->identifier->value)) {
      assert(target->identifier->value == set_attr_inst->base_var);
      auto x_f = new_obj->GetFiled(set_attr_inst->field);
      auto y = pfg->GetVariable(set_attr_inst->value_var, target->context);
      AddEdge(y, x_f);
    }
  }

private:
  std::map<Context *, const Function *> context_to_fn;
  CallGraph *cg;
  const Module *module_;
  const int k_cfa = 3;
};
#endif // POINTANALYSIS_CONTEXT_SENSITIVE_CS_PFG_BUILDER_H
