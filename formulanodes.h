
#pragma once

#include <string>
#include <iostream>
#include <cmath>
#include "avltree.h"
#include <map>



namespace global {
extern std::map<unsigned int, bool> Workspace;

extern void nextArgumentsSet();
}


class FormulaNode {
 public:
  virtual bool calc() const = 0;

  virtual void fillWorkspace() const = 0;

  virtual std::string str() const = 0;

  virtual FormulaNode *clone() const = 0;

  virtual ~FormulaNode() = default;
};

//------------------------------------------------------
// NumNode
//------------------------------------------------------
class NumNode  : public FormulaNode {
  const bool num;
 public:
  explicit NumNode(bool x) : num(x) {}

  bool calc() const  { return num; }

  void fillWorkspace() const  {}

  std::string str() const  {
    return std::to_string(num);
  }

  FormulaNode *clone() const  {
    return new NumNode(num);
  }
};

//------------------------------------------------------
// BinNode
//------------------------------------------------------
class BinNode : public FormulaNode {
 protected:
  FormulaNode *left, *right;
 public:
  BinNode(FormulaNode *L, FormulaNode *R) : left(L), right(R) {}

  ~BinNode() override {
    delete left;
    delete right;
    left = right = nullptr;
  }

  void fillWorkspace() const override {
    left->fillWorkspace();
    right->fillWorkspace();
  }
};

//------------------------------------------------------
// ConjunctionNode
//------------------------------------------------------
class ConjunctNode final : public BinNode {
 public:
  ConjunctNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  { return left->calc() && right->calc(); }

  std::string str() const  {
    return "(" + left->str() + " & " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new ConjunctNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// DisjunctionNode
//------------------------------------------------------
class DisjunctNode final : public BinNode {
 public:
  DisjunctNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  { return left->calc() || right->calc(); }

  std::string str() const  {
    return "(" + left->str() + " v " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new DisjunctNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// ImplicationNode
//------------------------------------------------------
class ImplicationNode final : public BinNode {
 public:
  ImplicationNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  { return !(left->calc()) || right->calc(); }

  std::string str() const  {
    return "(" + left->str() + " > " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new ImplicationNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// ConverseImplicationNode
//------------------------------------------------------
class ConverseImplicationNode final : public BinNode {
 public:
  ConverseImplicationNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  {
    return left->calc() || !(right->calc());
  }

  std::string str() const  {
    return "(" + left->str() + " < " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new ConverseImplicationNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// ExclusiveOrNode
//------------------------------------------------------
class ExclusiveOrNode final : public BinNode {
 public:
  ExclusiveOrNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  {
    return(left->calc()!=right->calc());
  }

  std::string str() const  {
    return "(" + left->str() + " + " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new ExclusiveOrNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// EquivalenceNode
//------------------------------------------------------
class EquivalenceNode final : public BinNode {
 public:
  EquivalenceNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  {
    return left->calc() == right->calc();
  }

  std::string str() const  {
    return "(" + left->str() + " = " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new EquivalenceNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// ShefferStrokeNode
//------------------------------------------------------
class ShefferStrokeNode final : public BinNode {
 public:
  ShefferStrokeNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  {
    bool l = left->calc(), r = right->calc();
    return !l || !r;
  }

  std::string str() const  {
    return "(" + left->str() + " | " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new ShefferStrokeNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// PiercesArrowNode
//------------------------------------------------------
class PierceArrowNode final : public BinNode {
 public:
  PierceArrowNode(FormulaNode *L, FormulaNode *R) : BinNode(L, R) {}

  bool calc() const  {
    bool l = left->calc(), r = right->calc();
    return !l && !r;
  }

  std::string str() const  {
    return "(" + left->str() + " ^ " + right->str() + ")";
  }

  FormulaNode *clone() const  {
    return new PierceArrowNode(left->clone(), right->clone());
  }
};

//------------------------------------------------------
// ParamNode
//------------------------------------------------------
class ParamNode final : public FormulaNode {
  const unsigned int ch;
 public:
  explicit ParamNode(unsigned int s) : ch(s) {}

  bool calc() const  {
    auto pos = global::Workspace.find(ch);
    if (pos != global::Workspace.end()) {
      return pos->second;
    } else {
      bool tmp;
      std::cout << "x" << ch << " = ";
      std::cin >> tmp;
      std::cin.ignore();
      global::Workspace.insert({ch, tmp});
      return tmp;
    }
  }

  void fillWorkspace() const  {
    auto pos = global::Workspace.find(ch);
    if (pos == global::Workspace.end()) {
      global::Workspace.insert({ch, false});
    }
  }

  std::string str() const  {
    return "x" + std::to_string(ch);
  }

  FormulaNode *clone() const  {
    return new ParamNode(ch);
  }
};

//------------------------------------------------------
// UnaryNode
//------------------------------------------------------
class UnaryNode  : public FormulaNode {
 protected:
  FormulaNode *next;
 public:
  explicit UnaryNode(FormulaNode *node) : next(node) {}

  ~UnaryNode() override {
    delete next;
    next = nullptr;
  }

  void fillWorkspace() const override {
    next->fillWorkspace();
  }
};

//------------------------------------------------------
// NegationNode
//------------------------------------------------------
class NegationNode final : public UnaryNode {
 public:
  explicit NegationNode(FormulaNode *node) : UnaryNode(node) {}

  bool calc() const  { return !next->calc(); }

  std::string str() const  {
    return "~" + next->str();
  }

  FormulaNode *clone() const  {
    return new NegationNode(next->clone());
  }
};