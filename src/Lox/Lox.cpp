#include "Lox/Lox.h"
#include <fstream>
#include <iostream>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

/**
 * @brief 运行指定路径的脚本文件。
 * 
 * 该函数尝试打开指定路径的文件，并读取其中的内容。
 * 如果文件无法打开，将输出错误信息并终止程序。
 * 
 * @param path 脚本文件的路径。
 */
void Lox::runFile(const std::string &path) {
    // 尝试打开指定路径的文件
    std::ifstream file(path);
    // 检查文件是否成功打开
    if (!file.is_open()) {
        // 使用LLVM的错误输出流输出错误信息
        llvm::errs() << "could not open the file" << path << "\n";
        // 报告致命错误并终止程序
        llvm::report_fatal_error("Failed to open the script file");
    }
}

/**
 * @brief 启动REPL（Read-Eval-Print Loop）以接受用户输入并即时运行lox代码。
 * 
 * 该函数创建一个交互式的命令行环境，允许用户输入Lox代码并立即执行。
 * 它会持续读取用户输入，直到用户终止输入（例如，通过Ctrl+D）。
 */
void Lox::runPrompt() {
    // 用于存储用户输入的每一行代码
    std::string line;
    // 进入无限循环，持续读取用户输入
    while (true) {
        // 使用LLVM的输出流输出提示符
        llvm::outs() << ">";
        // 刷新输出流，确保提示符立即显示
        llvm::outs().flush();
        // 从标准输入读取一行用户输入
        if (!std::getline(std::cin, line)) { 
            // 如果读取失败（例如，用户终止输入），跳出循环
            break; 
        }
        // 调用run函数执行用户输入的代码
        run(line);
        // 重置错误标记，准备下一次输入
        hadError = false;
    }
}

/**
 * 处理并运行lox代码。
 * @param line 包含lox代码的字符串。
 */
void Lox::run(const std::string &source) {
    // 这里通常会调用词法分析器、解析器和解释器
    // 现在只是简单地输出源代码
    llvm::outs() << source << "\n";// 使用LLVM的输出流

    // 示例错误处理
    if (source.find("error") != std::string::npos) { hadError = true; }
}



