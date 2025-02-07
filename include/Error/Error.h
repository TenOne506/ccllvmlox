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
     * @brief 报告错误。
     *
     * @param lint 错误发生的行号。
     * @param where 错误发生的位置描述。
     * @param message 错误消息。
     */
void report(int line, const llvm::StringRef &where, const llvm::StringRef &message);