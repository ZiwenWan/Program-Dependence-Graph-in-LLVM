#ifndef PDGEXCEPTIONS_H_
#define PDGEXCEPTIONS_H_
#include <exception>
#include <string>

namespace pdg
{
class DependencyNodeIsNullptrException : public std::exception
{
  std::string _msg;
  public:
    DependencyNodeIsNullptrException(const std::string& msg) : _msg(msg) {}
    virtual const char* what() const noexcept override {
      return _msg.c_str();
    }
};
} // namespace pdg

#endif