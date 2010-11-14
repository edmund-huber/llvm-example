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
#include <llvm/System/DynamicLibrary.h>
#include <llvm/Analysis/Passes.h>
using namespace llvm;
using namespace llvm::sys;

int host_method(int x){
  printf("hello, got x = %i\n", x);
  return 7 * x;
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
  sys::DynamicLibrary::AddSymbol("host_method", (void *)host_method);

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
  std::vector<GenericValue> args;
  GenericValue gv;
  gv.IntVal = APInt(32, 5, true);
  args.push_back(gv);
  GenericValue ret = engine->runFunction(client_method, args);
  printf("result was: %i\n", (int)ret.IntVal.getSExtValue());

  return 0;
}
