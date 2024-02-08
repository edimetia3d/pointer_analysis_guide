//
// License: MIT
//

#ifndef POINTANALYSIS_COMMON_PFG_BASE_H
#define POINTANALYSIS_COMMON_PFG_BASE_H
#include <map>

#include "node.h"
#include "object_base.h"

template <class StateType> struct PFGBase {
  explicit PFGBase(StateType &target_state) : state(target_state) {}
  Object *GetNewObject(const std::string type) {
    auto obj = std::make_unique<Object>(type);
    auto ret = obj.get();
    state.objects.insert(std::move(obj));
    return ret;
  }

  bool HasEdge(Node *src, Node *dst) {
    if (edges.find(src) == edges.end()) {
      return false;
    }
    return edges[src].find(dst) != edges[src].end();
  }

  void AddEdge(Node *src, Node *dst) { edges[src].insert(dst); }

  std::set<Node *> &GetChildren(Node *node) { return edges[node]; }

protected:
  StateType &state;

private:
  std::map<Node *, std::set<Node *>> edges;
};

#endif // POINTANALYSIS_COMMON_PFG_BASE_H
