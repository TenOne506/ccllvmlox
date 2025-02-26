#include "Lox/Interpreter.h"
#include "Lox/Lox.h"
#include "frontend/Parser.h"
#include "frontend/Resolver.h"
#include "frontend/Scanner.h"
#include <iostream>
#include <llvm/Support/CommandLine.h>
#include <vector>
#include <fstream>
using namespace llvm;
void printVector(const std::vector<int> &vec) {
    for (int iter: vec) { std::cout << iter << std::endl; }
}
cl::opt<std::string> InputFilename(cl::Positional, cl::desc("<input>"), cl::Required);

std::string read_string_from_file(const std::string &file_path) {
    const std::ifstream input_stream(file_path, std::ios_base::binary);

    if (input_stream.fail()) { throw std::runtime_error("Failed to open file"); }

    std::stringstream buffer;
    buffer << input_stream.rdbuf();

    return buffer.str();
}

int main(const int argc, char **argv) {
    cl::ParseCommandLineOptions(argc, argv);

    if (InputFilename.empty()) {
        std::cout << "source must not be empty";
        return 64;
    }

    Scanner Scanner(read_string_from_file(InputFilename));
    const auto &tokens = Scanner.scanTokens();
    Parser Parser(tokens);
    const auto &ast = Parser.parse();
    if (hadError) { return 65; }

    Resolver resolver;
    resolver.resolve(ast);
    if (hadError) { return 65; }


    Interpreter Interpreter;
    Interpreter.evaluate(ast);

    if (hadRuntimeError) { return 70; }


    return 0;
}
