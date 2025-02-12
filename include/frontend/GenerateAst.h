// 引入 LLVM 的 SmallVector 数据结构，用于存储词法单元等数据
#include <llvm/ADT/SmallVector.h>
// 引入 LLVM 的 StringRef 类型，用于高效处理字符串
#include <llvm/ADT/StringRef.h>
// 引入 LLVM 的错误处理机制
#include <llvm/Support/ErrorHandling.h>
// 引入 LLVM 的输出流，用于输出错误信息等
#include <llvm/Support/raw_ostream.h>

/**
 * @brief 用于生成抽象语法树（AST）相关代码的类。
 * 
 * 该类提供了静态方法 run 来启动 AST 代码生成过程，并包含了一些私有方法用于具体的代码生成操作。
 */
class GenerateAst {
public:
    /**
     * @brief 启动 AST 代码生成过程。
     * 
     * 该方法检查命令行参数的数量是否正确，如果不正确则输出使用说明并终止程序。
     * 如果参数数量正确，则将输出目录保存到静态成员变量 outputDir 中。
     * 
     * @param args 命令行参数，期望包含程序名和输出目录。
     */
    static void run(llvm::SmallVector<llvm::StringRef>& args){
        // 检查参数数量是否为 2，如果不是则输出使用说明
        if(args.size() != 2){
            // 输出错误信息到标准错误输出流
            llvm::errs()<<"Usage: generate_ast <output directory>"<<"\n";
            // 报告致命错误并终止程序
            llvm::report_fatal_error("Usage: generate_ast <output directory>");
            return;
        }
        // 将输出目录保存到静态成员变量中
        outputDir = args[1];
    }
private:
    // 静态成员变量，用于存储输出目录
    static llvm::StringRef outputDir;

    /**
     * @brief 定义抽象语法树（AST）的基本结构。
     * 
     * 该方法根据给定的输出目录、类型列表和基类名，生成 AST 的基本代码结构。
     * 
     * @param outtputDir 输出目录，指定生成的代码文件的保存位置。
     * @param types 类型列表，包含了 AST 中各种节点的类型。
     * @param baseName 基类名，所有 AST 节点类的基类名称。
     */
    void defineAst(llvm::StringRef outtputDir, llvm::SmallVector<llvm::StringRef>& types,llvm::StringRef baseName);

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
    void definteType(llvm::raw_ostream& outFile,llvm::StringRef basename,llvm::StringRef className,llvm::StringRef superClass);
};