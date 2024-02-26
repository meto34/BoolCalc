#pragma once

class Error {
 public:
  virtual std::string what() const {
    return std::string("error");
  }
};

//--------------------------------------------------------------------------
class ErrorAny : public Error {
 public:

  std::string what() const {

    return std::string("Error");
  }

};

