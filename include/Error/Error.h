#pragma once
#include "frontend/Token.h"
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>

static bool hadError = false;
static bool hadRuntimeError = false;
/**
 * @brief 报告错误信息到标准输出，并标记程序存在错误。
 * 
 * 该函数将错误信息格式化输出到标准输出流，包含错误所在的行号、位置和具体错误消息。
 * 同时，将全局变量 hadError 设置为 true，表示程序在运行过程中遇到了错误。
 * 
 * @param line 错误发生的行号，用于定位错误在源代码中的位置。
 * @param where 错误发生的具体位置描述，如具体的词法单元。
 * @param message 详细的错误消息，描述错误的具体情况。
 */
static void report(const long unsigned int line, const std::string_view where, const std::string_view message) {
    llvm::errs() << "[line " << line << "] Error" << where << ": " << message << "\n";
    hadError = true;
}

/**
 * @brief 报告指定行号的错误信息。
 * 
 * 该函数调用 report 函数，将错误信息输出到标准输出流。
 * 由于没有指定具体的错误位置，所以 where 参数为空字符串。
 * 
 * @param line 错误发生的行号，用于定位错误在源代码中的位置。
 * @param message 详细的错误消息，描述错误的具体情况。
 */
static void loxerror(const long unsigned int line, const std::string_view message) { report(line, "", message); }
static void error(const long unsigned int line, const std::string_view message) { report(line, "", message); }
/**
 * @brief 报告与特定词法单元相关的错误。
 * 
 * 该函数根据词法单元的类型，确定错误发生的具体位置描述，并调用 report 函数输出错误信息。
 * 如果词法单元类型为文件结束符（LoxEOF），则错误位置描述为 " at end"；
 * 否则，错误位置描述为 " at '" 加上词法单元的字面量表示。
 * 
 * @param token 与错误相关的词法单元，包含了错误发生位置的信息。
 * @param message 详细的错误消息，描述错误的具体情况。
 */
static void loxerror(const Token &token, const std::string_view message) {
    if (token.getType() == LoxEOF) {
        report(token.getLine(), " at end", message);
    } else {
        report(token.getLine(), " at '" + std::string(token.getLexeme()) + "'", message);
    }
}
static void error(const Token &token, const std::string_view message) {
    if (token.getType() == LoxEOF) {
        report(token.getLine(), " at end", message);
    } else {
        report(token.getLine(), " at '" + std::string(token.getLexeme()) + "'", message);
    }
}


struct runtime_error final : std::runtime_error {
    Token token;
    explicit runtime_error(const Token &token, const std::string &message)
        : std::runtime_error(message), token{token} {}
};

static void runtimeError(const runtime_error &error) {
    llvm::errs() << error.what() << "\n[line " << error.token.getLine() << "]";
    hadRuntimeError = true;
}