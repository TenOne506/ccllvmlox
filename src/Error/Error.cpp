#include <llvm/Support/raw_ostream.h>
#include "Error/Error.h"

void report(int line, const llvm::StringRef &where, const llvm::StringRef &message){
  llvm::outs()<<"[line "<<line<<"] Error"<<where<<": "<<message<<"\n";
  hadError = true;
}

void error(int line, const llvm::StringRef &message){
  report(line,"",message);
}


