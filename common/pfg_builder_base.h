//
// License: MIT
//

#ifndef POINTANALYSIS_COMMON_PFG_BUILDER_BASE_H
#define POINTANALYSIS_COMMON_PFG_BUILDER_BASE_H
#include <vector>

#include "ir.h"
#include "node.h"
PointeeSet SetDiff(const PointeeSet &a, const PointeeSet &b);
PointeeSet SetUnion(const PointeeSet &a, const PointeeSet &b);

std::set<AssignInst *> GetAllAssign(const std::set<Inst *> &all);
std::set<NewObjInst *> GetAllNewObj(const std::set<Inst *> &all);
std::set<SetAttrInst *> GetAllSetAttrOnVar(const std::set<Inst *> &all,
                                           const std::string &var_name);
std::set<GetAttrInst *> GetAllGetAttrOnVar(const std::set<Inst *> &all,
                                           const std::string &var_name);

template <class PFGType> struct PFGBuilderBase {
  explicit PFGBuilderBase(PFGType *pfg) : pfg(pfg) {}
  struct OutDatedNode {
    Node *node;
    PointeeSet new_pointees;
  };
  using WorkList = std::vector<OutDatedNode>;

  void AddEdge(Node *src, Node *dst) {
    if (pfg->HasEdge(src, dst)) {
      return; // If there is an edge, then propagate will keep the dst up to
              // date
    }
    pfg->AddEdge(src, dst);
    if (!src->pts.empty()) {
      wl.push_back({dst, src->pts});
    }
  }

  void Propagate(Node *target, const PointeeSet &new_pointee) {
    if (new_pointee.empty()) {
      return;
    }
    target->pts = SetUnion(target->pts, new_pointee);
    // Populate all target's children to worklist
    // Since they are all outdated
    for (auto child : pfg->GetChildren(target)) {
      wl.push_back({child, new_pointee});
    }
  }

protected:
  WorkList wl;
  PFGType *pfg;
};

#endif // POINTANALYSIS_COMMON_PFG_BUILDER_BASE_H
