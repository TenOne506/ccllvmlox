// 引入错误处理模块的头文件，包含错误处理相关的函数和类型定义
#include "Error/Error.h"
// 引入 LLVM 的输出流库，用于输出错误信息到标准输出
#include <llvm/Support/raw_ostream.h>

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
void report(int line, const llvm::StringRef &where, const llvm::StringRef &message) {
    // 格式化输出错误信息到标准输出流
    llvm::outs() << "[line " << line << "] Error" << where << ": " << message << "\n";
    // 标记程序存在错误
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
void error(int line, const llvm::StringRef &message) { 
    // 调用 report 函数报告错误，位置信息为空
    report(line, "", message); 
}

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
void error(Token token, llvm::StringRef message) {
    // 检查词法单元类型是否为文件结束符
    if (token.getType() == TokenType::LoxEOF) {
        // 如果是文件结束符，报告错误位置为文件末尾
        report(token.getLine(), " at end", message);
    } else {
        // 否则，报告错误位置为词法单元的字面量表示
        report(token.getLine(), " at '" + token.getLexeme().str() + "'", message);
    }
}
