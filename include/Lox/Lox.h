#pragma once
#include <cstdlib>// for exit()
#include <llvm/ADT/StringRef.h>
#include "Error/Error.h"

/**
 * @class Lox
 * @brief 表示Lox解释器的主类。
 * 
 * 该类提供了运行Lox代码的接口，支持从命令行交互式输入和从文件读取代码。
 */
class Lox {
public:
    /**
     * @brief 启动交互式命令行模式。
     * 
     * 该方法允许用户在命令行中逐行输入Lox代码并立即执行。
     */
    static void runPrompt();
    /**
     * @brief 从指定文件中读取并运行Lox代码。
     * 
     * @param path 包含Lox代码的文件的路径。
     */
    static void runFile(const llvm::StringRef &path);

    /**
     * @brief 运行给定的Lox源代码。
     * 
     * @param source 要执行的Lox源代码。
     */
    static void run(const llvm::StringRef &source);

   

private:
    // 标记在代码执行过程中是否发生了错误
    //这里统一错误处理
    //bool hadError = false;
};
