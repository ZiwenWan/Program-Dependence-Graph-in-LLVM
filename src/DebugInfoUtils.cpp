#include "DebugInfoUtils.hpp"

using namespace llvm;

std::string pdg::DIUtils::getArgName(Argument &arg)
{
  Function *F = arg.getParent();
  DISubprogram *dsp = F->getSubprogram();
  auto vars = dsp->getVariables();
  return vars[arg.getArgNo()]->getName().str();
}

DIType *pdg::DIUtils::getLowestDIType(DIType *Ty) 
{
  if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
      Ty->getTag() == dwarf::DW_TAG_member ||
      Ty->getTag() == dwarf::DW_TAG_typedef)
  {
    DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
    if (!baseTy)
    {
      errs() << "Type : NULL - Nothing more to do\n";
      return NULL;
    }

    //Skip all the DINodes with DW_TAG_typedef tag
    while ((baseTy->getTag() == dwarf::DW_TAG_typedef ||
            baseTy->getTag() == dwarf::DW_TAG_const_type ||
            baseTy->getTag() == dwarf::DW_TAG_pointer_type))
    {

      if (DITypeRef temp = dyn_cast<DIDerivedType>(baseTy)->getBaseType())
        baseTy = temp.resolve();
      else
        break;
    }
    return baseTy;
  }
  return Ty;
}

DIType *pdg::DIUtils::getArgDIType(Argument &arg)
{
  Function &F = *arg.getParent();
  SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
  F.getAllMetadata(MDs);
  for (auto &MD : MDs)
  {
    MDNode *N = MD.second;
    if (DISubprogram *subprogram = dyn_cast<DISubprogram>(N))
    {
      auto *subRoutine = subprogram->getType();
      const auto &TypeRef = subRoutine->getTypeArray();
      if (F.arg_size() >= TypeRef.size())
        break;
      const auto &ArgTypeRef = TypeRef[arg.getArgNo() + 1]; // + 1 to skip return type
      DIType *Ty = ArgTypeRef.resolve();
      return Ty;
    }
  }
  throw ArgHasNoDITypeException("Argument doesn't has DIType needed to extract field name...");
}

DIType *pdg::DIUtils::getFuncRetDIType(Function &F)
{
  SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
  F.getAllMetadata(MDs);
  for (auto &MD : MDs)
  {
    MDNode *N = MD.second;
    if (DISubprogram *subprogram = dyn_cast<DISubprogram>(N))
    {
      auto *subRoutine = subprogram->getType();
      const auto &TypeRef = subRoutine->getTypeArray();
      if (F.arg_size() >= TypeRef.size())
        break;
      const auto &ArgTypeRef = TypeRef[0];
      DIType *Ty = ArgTypeRef.resolve();
      return Ty;
    }
  }
  throw ArgHasNoDITypeException("Argument doesn't has DIType needed to extract field name...");
}

DIType *pdg::DIUtils::getBaseDIType(DIType *Ty) {
  if (Ty == nullptr)
    throw DITypeIsNullPtr("DIType is nullptr, cannot get base type");

  if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
      Ty->getTag() == dwarf::DW_TAG_member ||
      Ty->getTag() == dwarf::DW_TAG_typedef)
  {
    DIType *baseTy = dyn_cast<DIDerivedType>(Ty)->getBaseType().resolve();
    if (!baseTy)
    {
      errs() << "Type : NULL - Nothing more to do\n";
      return nullptr;
    }
    return baseTy;
  }
  return Ty;
}

std::string pdg::DIUtils::getDIFieldName(DIType *ty)
{
  // errs() << dwarf::TagString(ty->getTag()) << "\n";
  switch (ty->getTag())
  {
  case dwarf::DW_TAG_member:
  {
    return ty->getName().str();
  }
  case dwarf::DW_TAG_array_type:
  {
    ty = dyn_cast<DICompositeType>(ty)->getBaseType().resolve();
    return "arr_" + ty->getName().str();
  }
  case dwarf::DW_TAG_pointer_type:
  {
    std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
    return s;
  }
  case dwarf::DW_TAG_subroutine_type:
    return "func ptr";
  case dwarf::DW_TAG_const_type: {
    std::string s = getDIFieldName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
    return s;
  }
  default:
  {
    if (!ty->getName().str().empty())
      return ty->getName().str();
    return "no name";
  }
  }
}

std::string pdg::DIUtils::getDITypeName(DIType *ty)
{
  if (ty == nullptr)
    return "void";

  if (!ty->getTag() || ty == NULL) // process function type, which has not tag
  {
    std::string func_type_str = "";
    if (DISubroutineType *subRoutine = dyn_cast<DISubroutineType>(ty))
    {
      func_type_str += "( ";
      for (const auto &argTypeRef : subRoutine->getTypeArray())
      {
        DIType *d = argTypeRef.resolve();
        if (d == nullptr) // void type
        {
          func_type_str += "void ";
        }
        else
        {
          func_type_str += getDITypeName(d);
        }
      }
      func_type_str += " )";
      return func_type_str;
    }
    return "void";
  }
  try
  {
    switch (ty->getTag())
    {
    case dwarf::DW_TAG_typedef:
      return getDITypeName(getBaseDIType(ty));
    case dwarf::DW_TAG_member:
      return getDITypeName(getBaseDIType(ty));
    case dwarf::DW_TAG_array_type:
    {
      if (DIType *arrTy = dyn_cast<DICompositeType>(ty)->getBaseType().resolve())
        return arrTy->getName().str() + "[]";
    }
    case dwarf::DW_TAG_pointer_type:
    {
      std::string s = getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
      return s + "*";
    }
    case dwarf::DW_TAG_subroutine_type:
      return "func";
    case dwarf::DW_TAG_union_type:
      return "union";
    case dwarf::DW_TAG_structure_type:
      return "struct";
    case dwarf::DW_TAG_const_type:
      return "const " + getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
    default:
    {
      if (!ty->getName().str().empty())
        return ty->getName().str();
      return "unknow";
    }
    }
  }
  catch (std::exception &e)
  {
    errs() << e.what();
    exit(0);
  }
}