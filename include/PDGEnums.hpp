#ifndef PDGENUMS_H_
#define PDGENUMS_H_

namespace pdg
{
enum class DependencyType
{
  CALL,
  CONTROL,
  DATA_GENERAL,
  DATA_DEF_USE,
  DATA_RAW,
  DATA_READ,
  DATA_ALIAS,
  DATA_CALL_PARA,
  PARAMETER,
  STRUCT_FIELDS,
  NO_DEPENDENCY,
  GLOBAL_DEP
};

enum class GraphNodeType
{
  INST,
  FORMAL_IN,
  FORMAL_OUT,
  ACTUAL_IN,
  ACTUAL_OUT,
  RETURN,
  POINTER_RW,
  PARAMETER_FIELD,
  ENTRY,
  STRUCT_FIELD,
  GLOBAL_VALUE
};

enum class ControlType
{
  CTRUE,
  CFALSE,
  OTHER
};

enum class AccessType
{
  NOACCESS = 0,
  READ,
  WRITE
};

enum class TreeType
{
  ACTUAL_IN_TREE,
  ACTUAL_OUT_TREE,
  FORMAL_IN_TREE,
  FORMAL_OUT_TREE
};

enum class ArgumentMatchType {
  NOTCONTAINED,
  CONTAINED,
  EQUAL
};

} // namespace pdg

#endif