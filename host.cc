#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <llvm/DerivedTypes.h>
#include <llvm/LLVMContext.h>
#include <llvm/Linker.h>
#include <llvm/Module.h>
#include <llvm/System/Path.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
using namespace llvm;
using namespace llvm::sys;

int host_method(int x){
  return x * 3;
}

int main(){

  // Create a module that we can execute ..
  Linker *linker = new Linker("host", "linked-client", getGlobalContext());
  bool falsity = false;

  // .. pull in the client code.
  if (linker->LinkInFile(Path("client.s.bc"), falsity)) {
    printf("llvm linker error: '%s'\n", linker->getLastError().c_str());
    return 1;
  }

  // .. pull in the client-exposed host code.
  FunctionType *ft = FunctionType::get(Type::getInt32Ty(getGlobalContext()), false);
  Function::Create(ft, Function::ExternalLinkage, Twine("host_method"), linker->getModule());
  // this is not working

  // Finalize the module.
  Module *module = linker->releaseModule();
 
  // Make a new interpreter based on that module, and find the client_method.
  ExecutionEngine *engine = EngineBuilder(module).create();
  Function *client_method = engine->FindFunctionNamed("client_method");
  if (client_method == NULL) {
    printf("Was not able to find 'client_method' function, aborting.\n");
    return 1;
  }

  // Call the client_method with arguments.
  std::vector<GenericValue> args(1);
  args[0].UIntPairVal.first = 4;
  engine->runFunction(client_method, args);

  return 0;
}
