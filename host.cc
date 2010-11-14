#include <llvm/LLVMContext.h>
#include <llvm/Module.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
using namespace llvm;

int main(){
  Module *module = new Module("client code", getGlobalContext());
  ExecutionEngine *engine = EngineBuilder(module).create();
  return 0;
}
