#ifndef PDGEXCEPTIONS_H_
#define PDGEXCEPTIONS_H_
#include <exception>
#include <string>

namespace pdg
{
class DependencyNodeIsNullptrException : public std::exception
{
private:
  std::string _msg;

public:
  DependencyNodeIsNullptrException(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class TreeNodeTypeIsNull : public std::exception
{
private:
  std::string _msg;

public:
  TreeNodeTypeIsNull(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class NullPtrException : public std::exception
{
private:
  std::string _msg;

public:
  NullPtrException(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class ArgHasNoDITypeException : public std::exception
{
private:
  std::string _msg;

public:
  ArgHasNoDITypeException(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class ArgParameterTreeSizeIsZero : public std::exception
{
private:
  std::string _msg;

public:
  ArgParameterTreeSizeIsZero(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class ArgWrapperIsNullPtr : public std::exception
{
private:
  std::string _msg;

public:
  ArgWrapperIsNullPtr(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};

class DITypeIsNullPtr : public std::exception
{
private:
  std::string _msg;

public:
  DITypeIsNullPtr(const std::string &msg) : _msg(msg) {}
  virtual const char *what() const noexcept override
  {
    return _msg.c_str();
  }
};
} // namespace pdg

#endif