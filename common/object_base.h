//
// License: MIT
//

#ifndef POINTANALYSIS_COMMON_OBJECT_BASE_H
#define POINTANALYSIS_COMMON_OBJECT_BASE_H
#include <map>
#include <memory>

#include "node.h"

struct ObjectFiled : public Node {
  using Node::Node;
};

struct Object : public Node {
  Object(const std::string type) : Node(""), type(type) {
    // Object could be treated as special pointer who always points to itself
    // and has no identifier. This will make the implementation cleaner.
    identifier = nullptr;
    pts.insert(this);
  }

  ObjectFiled *GetFiled(const std::string &field) {
    auto sym = Symbol::Intern(field);
    if (fields.find(sym) == fields.end()) {
      fields[sym] = std::make_unique<ObjectFiled>(field);
    }
    return fields[sym].get();
  }

  Function *GetMethod(const std::string &method) {
    // since each object will have a known type, it is easy to find the method
    // on the type.
    return nullptr;
  }

private:
  // All fields' lifetime is managed by the object, it's just a implementation
  // detail to make the code cleaner.
  std::string type;
  std::map<Symbol *, std::unique_ptr<ObjectFiled>> fields;
};

#endif // POINTANALYSIS_COMMON_OBJECT_BASE_H
