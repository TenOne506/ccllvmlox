#pragma  once

#include "Value.h"
#include <llvm/IR/IRBuilder.h>
class LoxBuilder : llvm::IRBuilder<> {

private:
    llvm::Function &Function;
};