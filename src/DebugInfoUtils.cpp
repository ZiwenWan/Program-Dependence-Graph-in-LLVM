#include "DebugInfoUtils.hpp"

using namespace llvm;

std::string pdg::DIUtils::getArgName(Argument& arg, std::vector<DbgDeclareInst*> dbgInstList)
{
  Function *F = arg.getParent();
  SmallVector<std::pair<unsigned, MDNode *>, 20> func_MDs;
  for (auto I : dbgInstList)
  {
    if (auto dbi = dyn_cast<DbgDeclareInst>(I))
      if (auto DLV = dyn_cast<DILocalVariable>(dbi->getVariable()))
        if (DLV->getArg() == arg.getArgNo() + 1 && !DLV->getName().empty())
          return DLV->getName().str();
  }

  return "no_name";
}

DIType *pdg::DIUtils::getLowestDIType(DIType *Ty) 
{
  if (Ty->getTag() == dwarf::DW_TAG_pointer_type ||
      Ty->getTag() == dwarf::DW_TAG_member ||
      Ty->getTag() == dwarf::DW_TAG_typedef ||
      Ty->getTag() == dwarf::DW_TAG_const_type)
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
      int metaDataLoc = 0;
      if (arg.getArgNo() != 100)
        metaDataLoc = arg.getArgNo() + 1;
      const auto &ArgTypeRef = TypeRef[metaDataLoc]; // + 1 to skip return type
      DIType *Ty = ArgTypeRef.resolve();
      return Ty;
    }
  }
  return nullptr;
  // throw ArgHasNoDITypeException("Argument doesn't has DIType needed to extract field name...");
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
  return nullptr;
  // throw ArgHasNoDITypeException("Argument doesn't has DIType needed to extract field name...");
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
  if (ty == nullptr)
    return "void";
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
  case dwarf::DW_TAG_const_type:
  {
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

std::string pdg::DIUtils::getFuncSigName(DIType *ty, std::string funcName)
{
  std::string func_type_str = "";
  if (DISubroutineType *subRoutine = dyn_cast<DISubroutineType>(ty))
  {
    const auto &typeRefArr = subRoutine->getTypeArray();
    DIType *retType = typeRefArr[0].resolve();
    if (retType == nullptr)
      func_type_str += "void ";
    else
      func_type_str += getDITypeName(retType);

    func_type_str += "(";
    if (!funcName.empty())
      func_type_str += "*";
    func_type_str += funcName;
    func_type_str += ")";

    func_type_str += "(";
    for (int i = 1; i < typeRefArr.size(); ++i)
    {
      DIType *d = typeRefArr[i].resolve();
      if (d == nullptr) // void type
      { 
        func_type_str += "void ";
      }
      else
      {
        if (DIDerivedType *dit = dyn_cast<DIDerivedType>(d))
        {
          auto baseType = dit->getBaseType().resolve();
          if (baseType == nullptr) 
            func_type_str += "void ";
          else if (baseType->getTag() == dwarf::DW_TAG_structure_type)
            func_type_str = func_type_str + "projection " + getDITypeName(d) + " " + getDIFieldName(d);
        }
        else
          func_type_str = func_type_str + getDITypeName(d) + " " + getDIFieldName(d);
      }
    }
    func_type_str += ")";
    return func_type_str;
  }
  return "void";
}

std::string pdg::DIUtils::getFuncDITypeName(DIType *ty, std::string funcName)
{
  if (ty == nullptr)
    return "void";

  if (!ty->getTag() || ty == NULL) // process function type, which has not tag
    return getFuncSigName(ty);

  try
  {
    switch (ty->getTag())
    {
    case dwarf::DW_TAG_typedef:
      return getFuncDITypeName(getBaseDIType(ty), funcName);
      // return ty->getName().str();
    case dwarf::DW_TAG_member:
      return getFuncDITypeName(getBaseDIType(ty), funcName);
    case dwarf::DW_TAG_array_type:
    {
      if (DIType *arrTy = dyn_cast<DICompositeType>(ty)->getBaseType().resolve()) {
        return arrTy->getName().str() + "[" + std::to_string(arrTy->getSizeInBits()) + "]";
      }
    }
    case dwarf::DW_TAG_pointer_type:
    {
      std::string s = getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
      return (s + "_" + funcName + "*");
    }
    case dwarf::DW_TAG_subroutine_type:
      return getFuncSigName(ty);
    case dwarf::DW_TAG_union_type:
      return "union";
    case dwarf::DW_TAG_structure_type:
    {
      std::string st_name = ty->getName().str();
      if (!st_name.empty())
        return  st_name;
        // return "struct " + st_name;
      return "struct";
    }
    case dwarf::DW_TAG_const_type:
      return getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
      // return "const " + getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
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

std::string pdg::DIUtils::getDITypeName(DIType *ty)
{
  if (ty == nullptr)
    return "void";

  if (!ty->getTag() || ty == NULL) // process function type, which has not tag
    return getFuncSigName(ty);

  try
  {
    switch (ty->getTag())
    {
    case dwarf::DW_TAG_typedef:
      return getDITypeName(getBaseDIType(ty));
      // return ty->getName().str();
    case dwarf::DW_TAG_member:
      return getDITypeName(getBaseDIType(ty));
    case dwarf::DW_TAG_array_type:
    {
      if (DIType *arrTy = dyn_cast<DICompositeType>(ty)->getBaseType().resolve())
        return arrTy->getName().str() + "[" + std::to_string(ty->getSizeInBits() / arrTy->getSizeInBits()) + "]";
    }
    case dwarf::DW_TAG_pointer_type:
    {
      std::string s = getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
      return s + "*";
    }
    case dwarf::DW_TAG_subroutine_type:
      return getFuncSigName(ty);
    case dwarf::DW_TAG_union_type:
      return "union";
    case dwarf::DW_TAG_structure_type:
    {
      std::string st_name = ty->getName().str();
      if (!st_name.empty())
        return st_name;
        // return "struct " + st_name;
      return "struct";
    }
    case dwarf::DW_TAG_const_type:
      return "const " + getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
      // return "const " + getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
    case dwarf::DW_TAG_enumeration_type:
      return "enum";
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

std::string pdg::DIUtils::getArgTypeName(Argument &arg)
{
  return getDITypeName(getArgDIType(arg));
}