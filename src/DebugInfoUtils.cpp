#include "DebugInfoUtils.hpp"
#include <queue>
#include <set>

using namespace llvm;

std::string pdg::DIUtils::getArgName(Argument& arg, std::vector<DbgInfoIntrinsic*> dbgInstList)
{
  Function *F = arg.getParent();
  SmallVector<std::pair<unsigned, MDNode *>, 20> func_MDs;
  for (auto dbgInst : dbgInstList)
  {
    DILocalVariable *DLV = nullptr;
    if (auto declareInst = dyn_cast<DbgDeclareInst>(dbgInst))
      DLV = declareInst->getVariable();
    if (auto valueInst = dyn_cast<DbgValueInst>(dbgInst))
      DLV = valueInst->getVariable();
    if (!DLV)
      continue;
    if (DLV->getArg() == arg.getArgNo() + 1 && !DLV->getName().empty() && DLV->getScope()->getSubprogram() == F->getSubprogram())
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
      return nullptr;

    //Skip all the DINodes with DW_TAG_typedef tag
    while ((baseTy->getTag() == dwarf::DW_TAG_typedef ||
            baseTy->getTag() == dwarf::DW_TAG_const_type ||
            baseTy->getTag() == dwarf::DW_TAG_pointer_type))
    {
      if (DIType *temp = dyn_cast<DIDerivedType>(baseTy)->getBaseType().resolve())
        return temp;
      else
        break;
    }
    return baseTy;
  }
  return Ty;
}

std::set<DbgInfoIntrinsic *> pdg::DIUtils::collectDbgInstInFunc(Function &F)
{
  std::set<DbgInfoIntrinsic *> ret;
  for (auto instI = inst_begin(&F); instI != inst_end(&F); ++instI)
  {
    if (DbgInfoIntrinsic *dbi = dyn_cast<DbgInfoIntrinsic>(&*instI))
      ret.insert(dbi);
  }
  return ret;
}

DIType *pdg::DIUtils::getArgDIType(Argument &arg)
{
  Function &F = *arg.getParent();
  std::set<DbgInfoIntrinsic*> dbgInsts = collectDbgInstInFunc(F);
  for (auto dbgInst : dbgInsts)
  {
    DILocalVariable *DLV = nullptr;
    if (auto declareInst = dyn_cast<DbgDeclareInst>(dbgInst))
      DLV = declareInst->getVariable();
    if (auto valueInst = dyn_cast<DbgValueInst>(dbgInst))
      DLV = valueInst->getVariable();
    if (!DLV)
      continue;
    if (DLV->getArg() == arg.getArgNo() + 1 && !DLV->getName().empty() && DLV->getScope()->getSubprogram() == F.getSubprogram())
      return DLV->getType().resolve();
  }
  return nullptr;
}

// DIType *pdg::DIUtils::getArgDIType(Argument &arg)
// {
//   Function &F = *arg.getParent();
//   SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
//   F.getAllMetadata(MDs);
//   for (auto &MD : MDs)
//   {
//     MDNode *N = MD.second;
//     if (DISubprogram *subprogram = dyn_cast<DISubprogram>(N))
//     {
//       auto *subRoutine = subprogram->getType();
//       const auto &TypeRef = subRoutine->getTypeArray();
//       if (F.arg_size() >= TypeRef.size())
//         break;
//       int metaDataLoc = 0;
//       if (arg.getArgNo() != 100)
//         metaDataLoc = arg.getArgNo() + 1;
//       const auto &ArgTypeRef = TypeRef[metaDataLoc]; // + 1 to skip return type
//       DIType *Ty = ArgTypeRef.resolve();
//       return Ty;
//     }
//   }
//   return nullptr;
//   // throw ArgHasNoDITypeException("Argument doesn't has DIType needed to extract field name...");
// }

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

DIType *pdg::DIUtils::getGlobalVarDIType(GlobalVariable &globalVar)
{
  SmallVector<DIGlobalVariableExpression *, 1> GVs;
  globalVar.getDebugInfo(GVs);
  if (GVs.size() == 0)
    return nullptr;

  for (auto GV : GVs)
  {
    DIGlobalVariable* digv = GV->getVariable();
    return digv->getType().resolve();
  }
  return nullptr;
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

std::string pdg::DIUtils::getFuncSigName(DIType *ty, Function *F, std::string funcPtrName, std::string funcName, bool callFromDev)
{
  std::string func_type_str = "";
  if (DISubroutineType *subRoutine = dyn_cast<DISubroutineType>(ty))
  {
    const auto &typeRefArr = subRoutine->getTypeArray();
    // generate name string for return value
    DIType *retType = typeRefArr[0].resolve();
    if (retType == nullptr)
      func_type_str += "void ";
    else
      func_type_str += getDITypeName(retType);

    // generate name string for function pointer 
    func_type_str += " (";
    if (!funcPtrName.empty())
      func_type_str += "*";
    func_type_str += funcPtrName;
    if (!funcName.empty())
      func_type_str = func_type_str + "_" + funcName;
    func_type_str += ")";
    // generate name string for arguments in fucntion pointer signature
    func_type_str += "(";
    for (int i = 1; i < typeRefArr.size(); ++i)
    {
      DIType *d = typeRefArr[i].resolve();
      // retrieve naming info from debugging information for each argument
      std::string argName = getDIFieldName(d);
      if (F != nullptr)
      {
        unsigned argNum = i - 1;
        unsigned ptr = 0;
        for (auto argI = F->arg_begin(); argI != F->arg_end(); ++argI)
        {
          if (ptr == argNum)
          {
            auto dbgInstList = collectDbgInstInFunc(*F);
            std::vector<DbgInfoIntrinsic *> dbgList(dbgInstList.begin(), dbgInstList.end());
            argName = getArgName(*argI, dbgList);
            break;
          }
          ptr++;
        }
      }
    
      if (d == nullptr) // void type
        func_type_str += "void ";
      else // normal types
      {
        if (DIDerivedType *dit = dyn_cast<DIDerivedType>(d))
        {
          auto baseType = dit->getBaseType().resolve();
          if (!baseType)
            continue;
          else if (baseType->getTag() == dwarf::DW_TAG_structure_type)
          {
            std::string argTyName = getDITypeName(d);
            if (argTyName.back() == '*')
            {
              argTyName.pop_back();
              argTyName = argTyName + "_" + funcPtrName + "*";
            }
            else
            {
              argTyName = argTyName + "_" + funcPtrName;
            }

            std::string structName = argTyName + " " + argName;
            if (structName != " ")
              func_type_str = func_type_str + "projection " + structName;
          }
          else
            func_type_str = func_type_str + getDITypeName(d) + " " + argName;
        }
        else
          func_type_str = func_type_str + getDITypeName(d) + " " + argName;
      }

      if (i < typeRefArr.size() - 1 && !getDITypeName(d).empty())
        func_type_str += ", ";
    }
    func_type_str += ")";
    return func_type_str;
  }
  return "void";
}

// std::string pdg::DIUtils::getFuncDITypeName(DIType *ty, std::string funcName)
// {
//   if (ty == nullptr)
//     return "void";

//   if (!ty->getTag() || ty == NULL) // process function type, which has not tag
//     return getFuncSigName(ty);

//   try
//   {
//     switch (ty->getTag())
//     {
//     case dwarf::DW_TAG_typedef:
//       return getFuncDITypeName(getBaseDIType(ty), funcName);
//       // return ty->getName().str();
//     case dwarf::DW_TAG_member:
//       return getFuncDITypeName(getBaseDIType(ty), funcName);
//     case dwarf::DW_TAG_array_type:
//     {
//       if (DIType *arrTy = dyn_cast<DICompositeType>(ty)->getBaseType().resolve()) {
//         return arrTy->getName().str() + "[" + std::to_string(arrTy->getSizeInBits()) + "]";
//       }
//     }
//     case dwarf::DW_TAG_pointer_type:
//     {
//       std::string s = getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
//       return (s + "_" + funcName + "*");
//     }
//     case dwarf::DW_TAG_subroutine_type:
//       return getFuncSigName(ty);
//     case dwarf::DW_TAG_union_type:
//       return "union";
//     case dwarf::DW_TAG_structure_type:
//     {
//       std::string st_name = ty->getName().str();
//       if (!st_name.empty())
//         return  st_name;
//         // return "struct " + st_name;
//       return "struct";
//     }
//     case dwarf::DW_TAG_const_type:
//       return getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
//       // return "const " + getFuncDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve(), funcName);
//     default:
//     {
//       if (!ty->getName().str().empty())
//         return ty->getName().str();
//       return "unknow";
//     }
//     }
//   }
//   catch (std::exception &e)
//   {
//     errs() << e.what();
//     exit(0);
//   }
// }

std::string pdg::DIUtils::getDITypeName(DIType *ty)
{
  std::map<std::string, std::string> typeSwitchMap;
  typeSwitchMap.insert(std::make_pair("_Bool", "bool"));
  typeSwitchMap.insert(std::make_pair("long long unsigned int", "long"));

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
    case dwarf::DW_TAG_member:
      return getDITypeName(getBaseDIType(ty));
    case dwarf::DW_TAG_array_type:
    {
      if (DIType *arrTy = dyn_cast<DICompositeType>(ty)->getBaseType().resolve())
      {
        auto arrName = arrTy->getName().str();
        if (arrTy->getSizeInBits() != 0)
          return arrName + "[" + std::to_string(ty->getSizeInBits() / arrTy->getSizeInBits()) + "]";
        else
          return arrName + "[]";
      }
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
      return "struct";
    }
    case dwarf::DW_TAG_const_type:
      // return "const " + getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
      return getDITypeName(dyn_cast<DIDerivedType>(ty)->getBaseType().resolve());
    case dwarf::DW_TAG_enumeration_type:
      return "enum";
    default:
    {
      std::string typeName = ty->getName().str();
      if (!typeName.empty())
      {
        if (typeSwitchMap.find(typeName) != typeSwitchMap.end())
          return typeSwitchMap[typeName];
        return typeName;
      }
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

void pdg::DIUtils::printStructFieldNames(DINodeArray DINodeArr)
{
  for (auto DINode : DINodeArr)
    errs() << dyn_cast<DIType>(DINode)->getName() << "\n";
}

bool pdg::DIUtils::isPointerType(DIType *dt)
{
  if (dt == nullptr)
    return false;
  dt = stripMemberTag(dt);
  if (dt != nullptr)
    return (dt->getTag() == dwarf::DW_TAG_pointer_type);
  return false;
}

bool pdg::DIUtils::isStructPointerTy(DIType *dt)
{
  if (dt == nullptr)
    return false;
  dt = stripMemberTag(dt);
  if (dt->getTag() == dwarf::DW_TAG_pointer_type) {
    auto baseTy = getLowestDIType(dt);
    if (baseTy != nullptr) 
      return (baseTy->getTag() == dwarf::DW_TAG_structure_type);
  }
  return false;
}

bool pdg::DIUtils::isUnionPointerTy(DIType *dt)
{
  if (dt == nullptr)
    return false;
  dt = stripMemberTag(dt);
  if (dt->getTag() == dwarf::DW_TAG_pointer_type) {
    auto baseTy = getLowestDIType(dt);
    if (baseTy != nullptr) 
      return (baseTy->getTag() == dwarf::DW_TAG_union_type);
  }
  return false;
}

bool pdg::DIUtils::isStructTy(DIType *dt)
{
  if (dt == nullptr)
    return false;
  if (dt->getTag() == dwarf::DW_TAG_pointer_type)
    return false;
  auto baseTy = getLowestDIType(dt); // strip off tag member type
  if (baseTy != nullptr)
    return (baseTy->getTag() == dwarf::DW_TAG_structure_type);
  return false;
}

bool pdg::DIUtils::isFuncPointerTy(DIType *dt)
{
  if (dt == nullptr)
    return false;
  if (dt->getTag() == dwarf::DW_TAG_subroutine_type)
    return true;
  dt = stripMemberTag(dt);
  if (dt->getTag() == dwarf::DW_TAG_pointer_type) {
    auto baseTy = getBaseDIType(dt);
    if (baseTy != nullptr)
      return (getBaseDIType(dt)->getTag() == dwarf::DW_TAG_subroutine_type);
  }
  return false;
}

DIType *pdg::DIUtils::stripMemberTag(DIType *dt)
{
  if (dt == nullptr)
    return dt;
  if (dt->getTag() == dwarf::DW_TAG_member)
    return getBaseDIType(dt);
  return dt;
}

DIType *pdg::DIUtils::getFuncDIType(Function* func)
{
  auto subProgram = func->getSubprogram();
  return subProgram->getType();
}

std::vector<Function *> pdg::DIUtils::collectIndirectCallCandidatesWithDI(DIType *funcDIType, Module *module, std::map<std::string, std::string> funcptrTargetMap)
{
  std::vector<Function *> indirectCallList;
  for (auto &F : *module)
  {
    std::string funcName = F.getName().str();
    // get Function type
    if (funcName == "main" || !F.getSubprogram() || F.isDeclaration())
      continue;
    // compare the indirect call function type with each function, filter out certian functions that should not be considered as call targets
    if (funcptrTargetMap[DIUtils::getDIFieldName(funcDIType)] == F.getName())
    {
      indirectCallList.push_back(&F);
    }
  }

  return indirectCallList;
}

DIType *pdg::DIUtils::getInstDIType(Instruction* inst, std::vector<DbgInfoIntrinsic *> dbgInstList)
{
 for (auto dbgInst : dbgInstList)
  {
    if (dbgInst->getVariableLocation() != inst)
      continue;
    DILocalVariable *DLV = nullptr;
    if (auto declareInst = dyn_cast<DbgDeclareInst>(dbgInst))
      DLV = declareInst->getVariable();
    if (auto valueInst = dyn_cast<DbgValueInst>(dbgInst))
      DLV = valueInst->getVariable();
    if (!DLV)
      continue;
    return DLV->getType().resolve();
  }
  return nullptr;
}

DbgDeclareInst* pdg::DIUtils::getDbgInstForInst(Instruction* inst, std::set<DbgDeclareInst *> dbgInstList)
{
 for (auto dbi : dbgInstList)
  {
    if (dbi->getVariableLocation() != inst)
      continue;
    if (auto DLV = dyn_cast<DILocalVariable>(dbi->getVariable()))
      return dbi;
  }
  return nullptr;
}

std::set<std::string> pdg::DIUtils::computeSharedDataType(Module& M, std::set<Function*> crossDomainFunctions)
{
  std::set<std::string> sharedDataTypes;
  // compute shared type for global variables
  for (Module::global_iterator globalIt = M.global_begin(); globalIt != M.global_end(); ++globalIt)
  {
    if (GlobalVariable *globalVar = dyn_cast<GlobalVariable>(&*globalIt))
    {
      // 2. check if a global var is of struct pointer type
      DIType* globalVarDIType = getGlobalVarDIType(*globalVar);
      if (!globalVarDIType) 
        continue;
      if (isStructPointerTy(globalVarDIType))
      {
        sharedDataTypes.insert(getDIFieldName(globalVarDIType));
      }
    }
  }

  // compute shared data type for functions
  for (auto F : crossDomainFunctions)
  {
    if (F->isDeclaration() || F->empty())
      continue;
    for (auto &arg : F->args())
    {
      // do not process non-struct ptr type, struct type is coersed
      DIType *argDIType = DIUtils::getArgDIType(arg);
      if (!DIUtils::isStructPointerTy(argDIType))
        continue;
      // check if shared fields for this struct type is already done
      std::string argTypeName = DIUtils::getArgTypeName(arg);
      sharedDataTypes.insert(argTypeName);
    }
  }
  return sharedDataTypes;
}

std::string pdg::DIUtils::computeFieldID(DIType *rootDIType, DIType *fieldDIType)
{
  std::string rootName = "";
  std::string childName = "";
  if (rootDIType != nullptr)
    rootName = DIUtils::getDIFieldName(rootDIType);
  if (fieldDIType != nullptr)
    rootName = DIUtils::getDIFieldName(fieldDIType);
  std::string id =  rootName + childName;
  return id;
}

std::string pdg::DIUtils::getInvalidTypeStr(DIType* dt)
{
  std::queue<DIType*> typeQ;
  std::set<DIType*> seenType;
  typeQ.push(dt);
  while (!typeQ.empty())
  {
    DIType* dt = typeQ.front();
    typeQ.pop();
    // check for invalid type
    if (isUnionType(dt))
      return "union type";

    if (isArrayType(dt))
      return "array type";

    if (seenType.find(dt) != seenType.end())
      continue;
    seenType.insert(dt);
    if (dt->getTag() == dwarf::DW_TAG_structure_type)
    {
      auto DINodeArr = dyn_cast<DICompositeType>(dt)->getElements();
      for (unsigned i = 0; i < DINodeArr.size(); ++i)
      {
        if (DIType *tmpDI = dyn_cast<DIType>(DINodeArr[i]))
          typeQ.push(tmpDI);
      }
    }
    DIType* baseTy = getBaseDIType(dt);
    if (baseTy != nullptr && baseTy != dt)
      typeQ.push(baseTy);
  }
  return "";
}

bool pdg::DIUtils::isUnionType(DIType *dt)
{
  if (dt == nullptr)
    return false;
  if (dt->getTag() == dwarf::DW_TAG_pointer_type)
    return false;
  auto baseTy = getLowestDIType(dt); // strip off tag member type
  if (baseTy != nullptr)
    return (baseTy->getTag() == dwarf::DW_TAG_union_type);
  return false;
}

bool pdg::DIUtils::isArrayType(DIType *dt)
{
  return (dt->getTag() == dwarf::DW_TAG_array_type);
}