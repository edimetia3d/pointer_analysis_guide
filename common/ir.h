//
// License: MIT
//

#ifndef POINTANALYSIS_CONTEXT_INSENSITIVE_IR_H
#define POINTANALYSIS_CONTEXT_INSENSITIVE_IR_H

#include <set>
struct Inst {};

struct Function {
  std::string name;
  std::vector<std::string> params;
  std::vector<std::string> param_types;
  std::string ret_type;
  std::set<Inst *> insts;
};

struct Module {
  std::set<Function *> functions;
  Function *Lookup(const std::string &name) const;
  Function *LookupMethod(const std::string type,
                         const std::string &method) const;
};

struct NewObjInst : public Inst {
  std::string type;
  std::string result_var;
};

struct AssignInst : public Inst {
  std::string value_var;
  std::string result_var;
};

struct GetAttrInst : public Inst {
  std::string base_var;
  std::string field;
  std::string result_var;
};

struct SetAttrInst : public Inst {
  std::string base_var;
  std::string field;
  std::string value_var;
};

struct CallInst : public Inst {
  std::string callee;
  std::vector<std::string> args;
  std::string result_var;
};
struct CallMethod : public Inst {
  std::string base_var;
  std::string method;
  std::vector<std::string> args;
  std::string result_var;
};

struct RetInst : public Inst {
  std::string value_var;
};

#endif // POINTANALYSIS_CONTEXT_INSENSITIVE_IR_H
