
#pragma once

#include "formulanodes.h"
#include "error.h"
#include <fstream>
#include <stack>
#include <cmath>
#include <vector>
#include <cstring>
#include <iomanip>
#include <algorithm>
class BooleanExpression {
  unsigned int truthTableSize;
  unsigned int N;
  bool *truthTable;
  bool *zhegalkinCoefficients;
  FormulaNode *root;

  FormulaNode *Postfix2Tree_(const char *);

  void Infix2Postfix_(const char *, char *);

  void InfixFilter_(const char *, char *);//убираем табуляцию и пробелы

  static const unsigned char ActionsTable[][7];

  static int ActionsRowNumber(char ch);

  static int ActionsColNumber(char ch);

  void fillWorkspace();

  void calcTruthTable();

  void calcJeg();

 public:
  FormulaNode * GetRoot();

  explicit BooleanExpression(const char *str = "");

  explicit BooleanExpression(FormulaNode *r);

  BooleanExpression(const BooleanExpression &other);

  BooleanExpression(BooleanExpression &&other) noexcept;

  BooleanExpression &operator=(const BooleanExpression &copy);//copy constructor

  BooleanExpression &operator=(BooleanExpression &&move) noexcept;// move constructor

  ~BooleanExpression();

  bool calc() const;


  std::string str() const;

  explicit operator std::string() const;

  BooleanExpression dnf();

  BooleanExpression cnf();

  BooleanExpression zhegalkin();

  unsigned char getClassMask();

};

bool isFullSystem(std::vector<BooleanExpression> &);