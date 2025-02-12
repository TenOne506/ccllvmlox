#include "frontend/Token.h"
#include <llvm/ADT/StringRef.h>
static bool hadError = false;
/**
     * @brief 报告解析错误。
     *
     * @param line 错误发生的行号。
     * @param message 错误消息。
     */

void error(int line, const llvm::StringRef &message);


/**
 * @brief 报告与特定词法单元相关的错误。
 * 
 * 此函数用于在解析过程中，当遇到与某个特定词法单元相关的错误时，
 * 记录并输出错误信息。它会将错误标记为已发生，并输出错误所在的词法单元
 * 以及具体的错误消息。
 * 
 * @param token 与错误相关的词法单元，包含了错误发生位置的信息。
 * @param message 错误消息，描述了具体的错误情况。
 */
void error(Token token, llvm::StringRef message);

/**
     * @brief 报告错误。
     *
     * @param lint 错误发生的行号。
     * @param where 错误发生的位置描述。
     * @param message 错误消息。
     */
void report(int line, const llvm::StringRef &where, const llvm::StringRef &message);