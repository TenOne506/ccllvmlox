// 引入生成抽象语法树的头文件
#include "frontend/GenerateAst.h"
// 引入 LLVM 的 StringRef 类型，用于高效处理字符串
#include <llvm/ADT/StringRef.h>
// 引入系统错误处理库
#include <system_error>
// 引入 LLVM 的输出流，用于输出信息
#include "llvm/Support/raw_ostream.h"
// 引入 LLVM 的文件系统操作库
#include "llvm/Support/FileSystem.h"

/**
 * @brief 将输入字符串按指定分隔符分割，并去除分割后两部分的首尾空白字符。
 * 
 * 该函数接收一个输入字符串和一个分隔符，将输入字符串按分隔符分割为两部分，
 * 并将结果存储在引用参数 firstPart 和 secondPart 中。同时，去除这两部分的首尾空白字符。
 * 
 * @param input 输入的字符串，需要进行分割和处理。
 * @param delimiter 用于分割输入字符串的分隔符。
 * @param firstPart 引用参数，用于存储分割后的第一部分字符串。
 * @param secondPart 引用参数，用于存储分割后的第二部分字符串。
 */
void splitAndTrim(const std::string& input, const std::string& delimiter, std::string& firstPart, std::string& secondPart) {
    // 查找分隔符在输入字符串中的位置
    size_t pos = input.find(delimiter);
    // 如果未找到分隔符
    if (pos == std::string::npos) {
        // 输出错误信息到标准错误输出流
        llvm::errs() << "Delimiter not found in the input string.\n";
        return;
    }

    // 获取分割后的两部分
    firstPart = input.substr(0, pos);
    secondPart = input.substr(pos + delimiter.size());

    // 去除两端空白字符
    firstPart.erase(firstPart.find_last_not_of(" \n\r\t") + 1); // 删除尾部空格
    firstPart.erase(0, firstPart.find_first_not_of(" \n\r\t")); // 删除头部空格
    secondPart.erase(secondPart.find_last_not_of(" \n\r\t") + 1); // 删除尾部空格
    secondPart.erase(0, secondPart.find_first_not_of(" \n\r\t")); // 删除头部空格
}

/**
 * @brief 定义抽象语法树（AST）的基本结构。
 * 
 * 该方法根据给定的输出目录、类型列表和基类名，生成 AST 的基本代码结构。
 * 
 * @param outtputDir 输出目录，指定生成的代码文件的保存位置。
 * @param types 类型列表，包含了 AST 中各种节点的类型。
 * @param baseName 基类名，所有 AST 节点类的基类名称。
 */
void GenerateAst::defineAst(llvm::StringRef outtputDir, llvm::SmallVector<llvm::StringRef>& types,llvm::StringRef baseName){
    // 构建输出文件的路径
    std::string path = outtputDir.str() + "/" + baseName.str() + ".h";
    // 用于存储文件操作可能产生的错误代码
    std::error_code errorCode;
    // 打开输出文件流
    llvm::raw_fd_ostream outFile(path, errorCode, llvm::sys::fs::OF_Text);
    // 输出预处理指令，确保头文件只被包含一次
    outFile << "#pragma once\n";
    // 包含标准库中的智能指针头文件
    outFile << "#include <memory>\n";
    // 这里存在拼写错误，正确的应该是 abstract class
    outFile << "abstrcah class" << baseName << "{\n";
    // 结束基类的定义
    outFile << "}\n";
    // 关闭输出文件流
    //outFile.close();

    // 遍历类型列表
    for(llvm::StringRef type:types){
        // 存储分割后的类名
        std::string className;
        // 存储分割后的超类名
        std::string superClass;
        // 调用 splitAndTrim 函数进行分割和去除空白字符
        splitAndTrim(type.str(), ":", className, superClass); 
        // 这里 outFile 已经关闭，需要重新打开，否则会出错
        definteType(outFile, baseName, className, superClass);
    }
}

/**
 * @brief 定义 AST 中的具体类型。
 * 
 * 该方法根据给定的输出文件流、基类名、类名和超类名，生成具体 AST 节点类的代码。
 * 
 * @param outFile 输出文件流，用于将生成的代码写入文件。
 * @param basename 基类名，所有 AST 节点类的基类名称。
 * @param className 类名，当前要定义的 AST 节点类的名称。
 * @param superClass 超类名，当前 AST 节点类的父类名称。
 */
void GenerateAst::definteType(llvm::raw_ostream& outFile, llvm::StringRef basename, llvm::StringRef className, llvm::StringRef superClass){
    // 输出类定义，指定继承关系
    outFile << "class" << className << ":" << superClass << "{}\n";
    // 这里定义了一个未使用的变量 field，可考虑删除
    auto field = superClass.str();
    // 注释掉的代码，可能是用于生成构造函数的代码，但存在语法错误
    //outFile << "    " << className << "(" << superClass.str().split(",") << "){}\n";
}