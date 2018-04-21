#ifndef FUNCTIONWRAPPER_H
#define FUNCTIONWRAPPER_H

#include "llvm/Pass.h"
#include "DependencyGraph.h"
#include <list>
#include <vector>

//using namespace std;
using namespace llvm;


enum TreeType{
    ACTUAL_IN_TREE = 0,
    ACTUAL_OUT_TREE,
    FORMAL_IN_TREE,
    FORMAL_OUT_TREE
};


static int id = 0;


class TypeWrapper{
private:
    llvm::Type *ty;
    int id;
    //this flag is true means this type contains a field(or grandfield) that is function pointer or FILE*
    // bool flag;

public:
    TypeWrapper(llvm::Type* ty, int id){
        this->ty = ty;
        this->id = id;
        // this->flag = false;
    }

    llvm::Type* getType(){ return ty;}
    int getId(){return id;}
    // bool getUnseparatedFlag(){return flag;}
    //void setUnseparatedFlag(){this->flag = true;}

};

class ArgumentWrapper{

private:
    Argument *arg;

    //formal in/out trees should be together with each function body
    tree<InstructionWrapper*> formalInTree;
    tree<InstructionWrapper*> formalOutTree;
    //actual in/out trees should be together with each call wrapper
    tree<InstructionWrapper*> actualInTree;
    tree<InstructionWrapper*> actualOutTree;

    //  tree<InstructionWrapper*> testTree;

public:

    ArgumentWrapper(Argument* arg){
        this->arg = arg;
        //    this->recursive_types = {};
    }

    Argument* getArg(){
        return arg;
    }

    //TreeType: 0-ACTUAL_IN 1-ACTUAL_OUT 2-FORMAL_IN 3-FORMAL_OUT
    tree<InstructionWrapper*>& getTree(TreeType treeTy){
        switch(treeTy){

            case FORMAL_IN_TREE:
                return formalInTree;
            case FORMAL_OUT_TREE:
                return formalOutTree;
            case ACTUAL_IN_TREE:
                return actualInTree;
            case ACTUAL_OUT_TREE:
                return actualOutTree;

                break;
        }
    }

    void copyTree(const tree<InstructionWrapper*>& srcTree, TreeType treeTy){

        if(srcTree.empty()){

            errs() << *arg->getParent() << " arg : " << *arg << " srcTree is empty!\n";
            exit(1);
        }

        InstWrapperType instWTy;//formal_out actual_in/out

        switch(treeTy){
            case FORMAL_IN_TREE:
                errs() << "FORMAL_IN_TREE can't be copied\n";
                break;
            case FORMAL_OUT_TREE:
                formalOutTree = srcTree;
                instWTy = FORMAL_OUT;
                break;
            case ACTUAL_IN_TREE:
                actualInTree = srcTree;
                instWTy = ACTUAL_IN;
                break;
            case ACTUAL_OUT_TREE:
                actualOutTree = srcTree;
                instWTy = ACTUAL_OUT;
                break;
        }

        //    formalOutTree = formalInTree;
        tree<InstructionWrapper*>::iterator SI = srcTree.begin(), SE = srcTree.end();
        tree<InstructionWrapper*>::iterator TI = this->getTree(treeTy).begin(), TE = this->getTree(treeTy).end();

        //    InstructionWrapper(Function *Func, Argument *arg, Type* field_type, int field_id, InstWrapperType type) ;
        for(;SI != SE && TI != TE; ++SI, ++TI){
            InstructionWrapper * typeFieldW = new InstructionWrapper((*SI)->getFunction(), (*SI)->getArgument(),
                                                                     (*SI)->getFieldType(), id++, instWTy);
            *TI = typeFieldW;
            instnodes.insert(typeFieldW);
        }

    }//end copyTree



};



class CallWrapper{
private:
    CallInst *CI;
    std::list<ArgumentWrapper*> argWList;

public:

    CallWrapper(CallInst *CI) {

        this->CI = CI;
        //  Function::ArgumentListType& callee_args = Func->getArgumentList();
        for(Function::arg_iterator argIt = CI->getCalledFunction()->arg_begin(),
                    argE = CI->getCalledFunction()->arg_end(); argIt != argE; ++argIt){

            ArgumentWrapper *argW = new ArgumentWrapper(&*argIt);
            argWList.push_back(argW);
        }
    }

    CallInst* getCallInstruction(){
        return CI;
    }

    std::list<ArgumentWrapper*>& getArgWList(){
        return argWList;
    }
};

  
// FunctionWrapper
class FunctionWrapper {

private:
  Function *Func;
  InstructionWrapper *entryW;
  std::list<llvm::StoreInst *> storeInstList;
  std::list<llvm::LoadInst *> loadInstList;
  std::list<llvm::Instruction *> returnInstList;
  std::list<llvm::CallInst *> callInstList;
  std::list<ArgumentWrapper *> argWList;
  std::set<llvm::Value *> ptrSet;

  bool treeFlag = false;
  bool visited = false;

public:
  FunctionWrapper(Function *Func) {

    this->Func = Func;
    //  Function::ArgumentListType& callee_args = Func->getArgumentList();
    for (Function::arg_iterator argIt = Func->arg_begin(),
                                argE = Func->arg_end();
         argIt != argE; ++argIt) {

      ArgumentWrapper *argW = new ArgumentWrapper(&*argIt);
      argWList.push_back(argW);
    }
  }

  bool hasTrees() { return treeFlag; }

  void setTreeFlag(bool flag) { this->treeFlag = flag; }

  bool isVisited() { return visited; }

  void setVisited(bool flag) { this->visited = flag; }

  Function *getFunction() { return Func; }

  void setEntry(InstructionWrapper *entry) {
    //errs() << "Hello \n";
    this->entryW = entry;
  }

  InstructionWrapper *getEntry() { return entryW; }

  std::list<ArgumentWrapper *> &getArgWList() { return argWList; }

  std::list<llvm::StoreInst *> &getStoreInstList() { return storeInstList; }

  std::list<llvm::LoadInst *> &getLoadInstList() { return loadInstList; }

  std::list<llvm::Instruction *> &getReturnInstList() { return returnInstList; }

  std::list<llvm::CallInst *> &getCallInstList() { return callInstList; }

  std::set<llvm::Value *> &getPtrSet() { return ptrSet; }

  bool hasFuncOrFilePtr() {
    // check whether a function has a function_ptr or FILE parameter
    for (Function::arg_iterator argi = Func->arg_begin(),
                                arge = Func->arg_end();
         argi != arge; ++argi) {
      // params.push_back(argi->getType());
      // function ptr, put func into insensitive_set, not sensitive
      if (argi->getType()->isPointerTy()) {

        if (argi->getType()->getContainedType(0)->isFunctionTy()) {
          std::string Str;
          raw_string_ostream OS(Str);
          OS << *argi->getType();
          errs() << "DEBUG LINE 700 FunctionPointerType : " << OS.str() << "\n";
          return true;
        }

        if (argi->getType()->getContainedType(0)->isStructTy()) {
          std::string Str;
          raw_string_ostream OS(Str);
          OS << *argi->getType();

          // FILE*, bypass, no need to buildTypeTree
          if ("%struct._IO_FILE*" == OS.str() ||
              "%struct._IO_marker*" == OS.str()) {

            errs() << "DEBUG LINE 711 struct._IO_marker*/struct._IO_FILE* \n";
            return true;
            // continue;
          }
        }
      }
    } // end for Function::arg_iterator argi...

    return false;
  }
};

#endif // FUNCTIONWRAPPER_H
