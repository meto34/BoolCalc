
#include "boolexpr.h"


//-----------------------------------------------------
FormulaNode *BooleanExpression::Postfix2Tree_(const char *str) {
  int index = 0; // Номер элемента во входной строке
  std::stack<FormulaNode *> S;
  char ch;
  unsigned variableIndex, variableIndexLength;
  bool push = true;
  FormulaNode *result, *left, *right;
  try {
    while ((ch = str[index]) != '\0') {
      left = right = nullptr;
      switch (ch) {
        case '.':
          push = false;
          break;
        case '~':
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new NegationNode(left);
          break;
        case '&':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new ConjunctNode(left, right);
          break;
        case 'v':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new DisjunctNode(left, right);
          break;
        case '>':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new ImplicationNode(left, right);
          break;
        case '<':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new ConverseImplicationNode(left, right);
          break;
        case '+':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new ExclusiveOrNode(left, right);
          break;
        case '=':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new EquivalenceNode(left, right);
          break;
        case '|':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new ShefferStrokeNode(left, right);
          break;
        case '^':
          if (S.empty()) throw ErrorAny();
          right = S.top();
          S.pop();
          if (S.empty()) throw ErrorAny();
          left = S.top();
          S.pop();
          result = new PierceArrowNode(left, right);
          break;
        case 'x':
          variableIndex = 0;
          variableIndexLength = 0;
          ++index;
          while (('0' <= str[index]) && (str[index] <= '9')) {
            variableIndex = variableIndex * 10 + (str[index] - '0');
            ++variableIndexLength;
            ++index;
          }
          if (variableIndexLength == 0) throw ErrorAny();
          result = new ParamNode(variableIndex);
          --index;
          break;
        default:
          if (ch >= '0' && ch <= '9')
            result = new NumNode(ch - '0');
          else if ((ch >= 'a' && ch < 'v') || (ch > 'v' && ch <= 'z') ||
                   (ch >= 'A' && ch <= 'Z'))
            result = new ParamNode(ch);
      }
      if (push) {
        S.push(result);
      }
      push = true;

      ++index;
    }
    if (S.size() > 1) {
      left = right = nullptr;
      throw ErrorAny();
    }
    result = S.top();
    return result;
  }
  catch (int E) {
    if (left) delete left;
    if (right) delete right;
    while (!S.empty()) {
      left = S.top();
      S.pop();
      delete left;
    }
    throw ErrorAny();
  }
}

//-----------------------------------------------------

const unsigned char BooleanExpression::ActionsTable[][7] = {
    //\0 ~ & O ( ) P
    {7, 2, 2, 2, 2, 5, 1},// \0
    {3, 2, 3, 3, 2, 3, 1},// ~
    {3, 2, 3, 3, 2, 3, 1},// &
    {3, 2, 2, 3, 2, 3, 1},// O
    {6, 2, 2, 2, 2, 4, 1},// (
};

//-----------------------------------------------------

int BooleanExpression::ActionsRowNumber(char ch) {
  switch (ch) {
    case 0:
      return 0;
    case '~':
      return 1;
    case '&':
      return 2;
    case '(':
      return 4;
  }
  return 3;
}

//-----------------------------------------------------

int BooleanExpression::ActionsColNumber(char ch) {
  switch (ch) {
    case 0:
      return 0;
    case '~':
      return 1;
    case '&':
      return 2;
    case '(':
      return 4;
    case ')':
      return 5;
  }
  if ((ch >= 'a' && ch < 'v') || (ch > 'v' && ch <= 'z')) return 6;
  if (ch >= 'A' && ch <= 'Z') return 6;
  if (ch >= '0' && ch <= '9') return 6;
  return 3;
}

//-----------------------------------------------------

void BooleanExpression::Infix2Postfix_(const char *instr, char *outstr) {
  int i = 0, j = 0; // i - индекс во входной строке, j - в выходной
  int row, col;
  unsigned char action;
  std::stack<char> S;
  bool isP = false;
  do {
    col = ActionsColNumber(instr[i]);
    row = S.empty() ? 0 : ActionsRowNumber(S.top());
    action = ActionsTable[row][col];
    switch (action) {
      case 1:
        outstr[j] = instr[i];
        ++j;
        ++i;
        isP = true;
        break;
      case 2:
        S.push(instr[i]);
        ++i;
        if (isP) {
          outstr[j] = '.';
          ++j;
          isP = false;
        }
        break;
      case 3:
        outstr[j] = S.top();
        ++j;
        S.pop();
        break;
      case 4:
        S.pop();
        ++i;
        break;
      case 5:
        throw ErrorAny();
        break;
      case 6:
        throw ErrorAny();
        break;
      case 7:
        outstr[j] = '\0';
        break;
      case 8:
        throw ErrorAny();
        break;
      case 9:
        throw ErrorAny();
        break;
    }
  } while (action != 7);
}

void BooleanExpression::InfixFilter_(const char *instr, char *outstr) {
  int i = 0, j = 0;
  char ch, prev = 0;
  char buf[256];
  int bufLen = 0;
  while ((ch = instr[i++]) != 0) {
    // Пропуск пробелов и табуляций
    if (ch == ' ' || ch == '\t' || ch == '\r') continue;

    if (ch == 'x') {
      if (instr[i] == ' ' || instr[i] == '\t' || instr[i] == '\r') throw ErrorAny();
      if (bufLen == 1) {
        outstr[j++] = buf[0];
        bufLen = 0;
      }
      outstr[j++] = ch;
    } else if ((ch >= 'a' && ch < 'v') || (ch > 'v' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
               (ch >= '0' && ch <= '9')) {
      if (prev == 'x') {
        outstr[j++] = ch;
        continue;
      } else buf[bufLen++] = ch;
    } else {
      if (bufLen == 1) {
        outstr[j++] = buf[0];
        bufLen = 0;
      }
      outstr[j++] = ch;
    }
    prev = ch;
  }
  if (bufLen > 1) throw ErrorAny();
  if (bufLen > 0) outstr[j++] = buf[0];
  outstr[j] = '\0';
}

//-----------------------------------------------------------

void BooleanExpression::fillWorkspace() {
  if (root) root->fillWorkspace();
}

//-----------------------------------------------------------

void BooleanExpression::calcTruthTable() {
  if (truthTable != nullptr) return;
  fillWorkspace();
  truthTable = new bool[truthTableSize];
  for (int i = 0; i < truthTableSize; ++i) {
    truthTable[i] = calc();
    global::nextArgumentsSet();
  }
  global::Workspace.clear();
}

//-----------------------------------------------------------

void BooleanExpression::calcJeg() {
  calcTruthTable();
  if (zhegalkinCoefficients != nullptr) return;
  bool *currPascalTriangleCol = new bool[truthTableSize];
  for (
      bool *from = truthTable, *to = currPascalTriangleCol, *end = truthTable + truthTableSize;
      from != end;
      ++from, ++to
  ) {
    *to = *from;
  }
  zhegalkinCoefficients = new bool[truthTableSize];
  for (unsigned col = 0; col < truthTableSize; ++col) {
    zhegalkinCoefficients[col] = currPascalTriangleCol[0];
    for (unsigned row = 0; row < (truthTableSize - col - 1); ++row) {
      currPascalTriangleCol[row] = (currPascalTriangleCol[row] != currPascalTriangleCol[row + 1]);

    }
  }
}

//-----------------------------------------------------------

BooleanExpression::BooleanExpression(const char *str) : truthTable(nullptr), truthTableSize(0), N(0),
                                                        zhegalkinCoefficients(nullptr) {
  int length = std::strlen(str);
  length = std::max(20, 2 * length);
  char *str_infix = new char[length];
  char *str_postfix = new char[length];

  try {
    InfixFilter_(str, str_infix);
    Infix2Postfix_(str_infix, str_postfix);
    root = Postfix2Tree_(str_postfix);
  }
  catch (...) {
    delete[] str_postfix;
    delete[] str_infix;
    root = nullptr;
    throw ErrorAny();
  }
  fillWorkspace();
  N = global::Workspace.size();
  truthTableSize = 1 << N;
  global::Workspace.clear();
  delete[] str_infix;
  delete[] str_postfix;
}

//-----------------------------------------------------------

BooleanExpression::BooleanExpression(FormulaNode *r) : truthTable(nullptr), truthTableSize(0), N(0),
                                                       zhegalkinCoefficients(nullptr),
                                                       root(r) {}

//-----------------------------------------------------------

BooleanExpression::~BooleanExpression() {
  delete[] truthTable;
  delete[] zhegalkinCoefficients;
  delete root;
}

//-----------------------------------------------------------

bool BooleanExpression::calc() const {
  if (root) return root->calc();
  return false;
}



//-----------------------------------------------------------

std::string BooleanExpression::str() const {
  if (root) return root->str();
  return {""};
}

//-----------------------------------------------------------

BooleanExpression::operator std::string() const {
  if (root) return root->str();
  return {""};
}

//----------------------------------------------------------- //NumNode(false);

BooleanExpression BooleanExpression::dnf() {
  FormulaNode *result = nullptr;
  fillWorkspace();
  if (truthTableSize == 1) {
    bool check =(GetRoot()->calc());
    auto *tmp = new NumNode(check);
    result = tmp;
  } else {
    for (int i = 0; i < truthTableSize; ++i) {
      if (calc()) {
        FormulaNode *temp = nullptr;
        auto it = global::Workspace.begin();

        while (it != global::Workspace.end()) {
          if (temp == nullptr) {
            temp = new ParamNode(it->first);
            if (!it->second) {
              temp = new NegationNode(temp);
            }
          } else {
            temp = it->second
                       ? new ConjunctNode(temp, new ParamNode(it->first))
                       : new ConjunctNode(temp, new NegationNode(new ParamNode(it->first)));
          }
          ++it;
        }

        result = result == nullptr ? temp : new DisjunctNode(result, temp);
      }
      global::nextArgumentsSet();
    }
  }
  global::Workspace.clear();
  return BooleanExpression{result};
}

//-----------------------------------------------------------

BooleanExpression BooleanExpression::cnf() {
  FormulaNode *result = nullptr;
  fillWorkspace();
  if (truthTableSize == 1) {
    bool check =(GetRoot()->calc());
    auto *tmp = new NumNode(check);
    result = tmp;
  }else {


    for (int i = 0; i < truthTableSize; ++i) {
      if (!calc()) {
        FormulaNode *temp = nullptr;
        auto it = global::Workspace.begin();
        while (it != global::Workspace.end()) {
          if (temp == nullptr) {
            temp = new ParamNode(it->first);
            if (it->second) {
              temp = new NegationNode(temp);
            }
          } else {
            temp = it->second
                       ? new DisjunctNode(temp, new NegationNode(new ParamNode(it->first)))
                       : new DisjunctNode(temp, new ParamNode(it->first));
          }
          ++it;
        }
        result = result == nullptr
                     ? temp
                     : new ConjunctNode(result, temp);
      }
      global::nextArgumentsSet();
    }
  }
  global::Workspace.clear();
  return BooleanExpression{result};
}

BooleanExpression BooleanExpression::zhegalkin() {
  if (zhegalkinCoefficients == nullptr) {
    calcJeg();
  }
  FormulaNode *result = nullptr;
  fillWorkspace();
  if (truthTableSize == 1) {
    bool check =(GetRoot()->calc());
    auto *tmp = new NumNode(check);
    result = tmp;
  }else {
    for (int i = 0; i < truthTableSize; ++i) {
      if (zhegalkinCoefficients[i]) {
        FormulaNode *curr = nullptr;
        auto it = global::Workspace.begin();
        while (it != global::Workspace.end()) {
          if (it->second) {
            curr = (curr == nullptr)
                       ? static_cast<FormulaNode *>(new ParamNode(it->first))
                       : static_cast<FormulaNode *>(new ConjunctNode(curr, new ParamNode(it->first)));

          }
          ++it;
        }
        if (curr == nullptr) {
          curr = new NumNode(true);
        }
        result = (result == nullptr)
                     ? curr
                     : new ExclusiveOrNode(curr, result);
      }
      global::nextArgumentsSet();
    }
  }
  global::Workspace.clear();
  return BooleanExpression(result);
}

unsigned char BooleanExpression::getClassMask() {
  uint8_t classMask = 0b11111; // represents 5 classes:
  // monotonic, falsity-preserving, truth-preserving, linear, self-dual
  calcTruthTable();
  calcJeg();
  if (truthTable[0]) {
    classMask = classMask & (uint8_t) 0b11111101; // check T0 - false
  }
  if (!truthTable[truthTableSize - 1]) {
    classMask = classMask & (uint8_t) (0b11111011); // check T1 - truth
  }
  for (int i = 1; i < truthTableSize; ++i) {
    if (truthTable[i] < truthTable[i - 1])
      classMask = classMask & (uint8_t) (0b11111110); // check M - monotonic
  }
  for (int i = 0; i < truthTableSize / 2; ++i) {
    if (truthTable[i] != truthTable[truthTableSize - i - 1]) {
      classMask = classMask & (uint8_t) (0b11101111); // check S - self-dual
    }
  }
  for (int i = 3; i < truthTableSize; ++i) {
    if (((i & (i - 1)) != 0) && zhegalkinCoefficients[i]) {
      classMask = classMask & (uint8_t) (0b11110111); // check L
    }
  }
  return classMask;
}

BooleanExpression::BooleanExpression(const BooleanExpression &other) :
                                                                       truthTableSize(other.truthTableSize),
                                                                       N(other.N),
                                                                       truthTable(nullptr),
                                                                       zhegalkinCoefficients(nullptr),
                                                                       root(nullptr) {
  if (other.truthTable) {
    truthTable = new bool[truthTableSize];
    for (int i = 0; i < truthTableSize; ++i) {
      truthTable[i] = other.truthTable[i];
    }
  }
  if (other.zhegalkinCoefficients) {
    zhegalkinCoefficients = new bool[truthTableSize];
    for (int i = 0; i < truthTableSize; ++i) {
      zhegalkinCoefficients[i] = other.zhegalkinCoefficients[i];
    }
  }
  root = other.root ? other.root->clone() : nullptr;
}

BooleanExpression::BooleanExpression(BooleanExpression &&other) noexcept {
  truthTableSize = other.truthTableSize;
  N = other.N;
  truthTable = other.truthTable;
  zhegalkinCoefficients = other.zhegalkinCoefficients;
  root = other.root;
  other.truthTableSize = 0;
  other.N = 0;
  other.truthTable = nullptr;
  other.zhegalkinCoefficients = nullptr;
  other.root = nullptr;
}

BooleanExpression &BooleanExpression::operator=(const BooleanExpression &copy) {
  if (this != &copy) {
    delete[] truthTable;
    delete[] zhegalkinCoefficients;
    delete root;
    truthTableSize = copy.truthTableSize;
    N = copy.N;
    truthTable = new bool[truthTableSize];
    for (int i = 0; i < truthTableSize; ++i) {
      truthTable[i] = copy.truthTable[i];
    }
    zhegalkinCoefficients = new bool[truthTableSize];
    for (int i = 0; i < truthTableSize; ++i) {
      zhegalkinCoefficients[i] = copy.zhegalkinCoefficients[i];
    }
    root = copy.root ? copy.root->clone() : nullptr;
  }
  return *this;
}

BooleanExpression &BooleanExpression::operator=(BooleanExpression &&move) noexcept {
  if (this != &move) {
    delete[] truthTable;
    delete[] zhegalkinCoefficients;
    delete root;
    truthTableSize = move.truthTableSize;
    N = move.N;
    truthTable = move.truthTable;
    zhegalkinCoefficients = move.zhegalkinCoefficients;
    root = move.root;
    move.truthTableSize = 0;
    move.N = 0;
    move.truthTable = nullptr;
    move.zhegalkinCoefficients = nullptr;
    move.root = nullptr;
  }
  return *this;
}

FormulaNode *BooleanExpression::GetRoot() {
  return root;
}

//-----------------------------------------------------------

bool isFullSystem(std::vector<BooleanExpression> &expressions) {
  unsigned char ch = 0b11111;
  for (auto &exp: expressions) {
    ch = ch & exp.getClassMask();
  }
  return ch == 0;
}