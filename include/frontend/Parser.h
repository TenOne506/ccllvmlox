#pragma once
// 引入前端词法单元的头文件
#include "frontend/Token.h"
// 引入前端抽象语法树的头文件
#include "frontend/Ast.h"

// 引入 LLVM 的 SmallString 数据结构
#include <llvm/ADT/SmallString.h>
// 引入 LLVM 的 SmallVector 数据结构
#include <llvm/ADT/SmallVector.h>
#define MAX_PARAMETERS 255
/**
 * @brief 解析器类，用于将词法单元序列解析为抽象语法树（AST）。
 * 
 * 该类负责接收词法单元序列，并通过一系列的解析方法将其转换为抽象语法树。
 * 解析过程遵循一定的语法规则，从简单的表达式开始，逐步构建复杂的表达式。
 */
class Parser{

private:
    // 存储词法单元的向量
    llvm::SmallVector<Token> tokens;
    // 当前处理的词法单元的索引
    int current = 0;

    /**
     * @brief 解析表达式，从相等性表达式开始。
     * 
     * @return Expr 解析得到的表达式。
     */
    Expr expression();

    /**
     * @brief 解析相等性表达式，如 !=, ==。
     * 
     * @return Expr 解析得到的相等性表达式。
     */
    Expr equality();

    /**
     * @brief 检查当前词法单元是否匹配任意一个给定的类型，如果匹配则前进到下一个词法单元。
     * 
     * @tparam Args 可变参数模板，表示多个词法单元类型。
     * @param type 可变参数，表示多个词法单元类型。
     * @return true 如果匹配到任意一个类型。
     * @return false 如果没有匹配到任何类型。
     */
    template<typename... Args>
    bool match(Args... type);

    /**
     * @brief 检查当前词法单元是否为指定类型。
     * 
     * @param type 要检查的词法单元类型。
     * @return true 如果当前词法单元是指定类型。
     * @return false 如果当前词法单元不是指定类型。
     */
    bool check(TokenType type);

    /**
     * @brief 前进到下一个词法单元，并返回前一个词法单元。
     * 
     * @return Token 前一个词法单元。
     */
    Token advance();

    /**
     * @brief 获取当前位置的词法单元。
     * 
     * @return Token 当前位置的词法单元。
     */
    Token peek();

    /**
     * @brief 获取前一个位置的词法单元。
     * 
     * @return Token 前一个位置的词法单元。
     */
    Token previous();

    /**
     * @brief 检查是否已经到达词法单元序列的末尾。
     * 
     * @return true 如果已经到达末尾。
     * @return false 如果还未到达末尾。
     */
    bool isAtEnd();

    /**
     * @brief 解析比较表达式，如 >, >=, <, <=。
     * 
     * @return Expr 解析得到的比较表达式。
     */
    Expr comparison();

    /**
     * @brief 解析加减表达式。
     * 
     * @return Expr 解析得到的加减表达式。
     */
    Expr term();

    /**
     * @brief 解析乘除表达式。
     * 
     * @return Expr 解析得到的乘除表达式。
     */
    Expr factor();

    /**
     * @brief 解析一元表达式，如 ! 或 -。
     * 
     * @return Expr 解析得到的一元表达式。
     */
    Expr unary();

    /**
     * @brief 解析基本表达式，如字面量、括号表达式等。
     * 
     * @return Expr 解析得到的基本表达式。
     */
    Expr primary();

    /**
     * @brief 消耗一个指定类型的词法单元，如果当前词法单元类型匹配则前进到下一个词法单元，否则报错。
     * 
     * @param type 期望的词法单元类型。
     * @param message 当词法单元类型不匹配时显示的错误信息。
     * @return Token 消耗的词法单元。
     */
    Token consume(TokenType type, std::string_view message);

    /**
     * @brief 同步解析器的状态，跳过无效的词法单元直到找到合适的同步点。
     * 
     * 当解析过程中遇到错误时，调用此函数来尝试恢复解析。
     */
    void synchronize();


    std::optional<Stmt> declaration();

    ClassStmtPtr classDeclaration();

    VarStmtPtr varDeclaration();

    FunctionStmtPtr function(LoxFunctionType type);

    StmtList block();
public:
    /**
     * @brief 构造函数，初始化解析器并传入词法单元序列。
     * 
     * @param tokens 要解析的词法单元序列。
     */
    explicit Parser(llvm::SmallVector<Token> tokens);

    /**
     * @brief 开始解析过程，从表达式解析开始。
     * 
     * @return Expr 解析得到的表达式。
     */
    Expr parse();

    Program Parse();
};