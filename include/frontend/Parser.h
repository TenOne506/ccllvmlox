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

    /**
     * @brief 解析声明语句，可能是变量声明、函数声明、类声明等。
     * 
     * 该函数尝试解析一个声明语句，并返回一个可选的语句对象。
     * 如果解析成功，返回包含语句的 std::optional；如果解析失败，返回空的 std::optional。
     * 
     * @return std::optional<Stmt> 解析得到的声明语句，可能为空。
     */
    std::optional<Stmt> declaration();

    /**
     * @brief 解析类声明语句。
     * 
     * 该函数负责解析类声明语句，包括类名、类的成员等，并返回一个指向 ClassStmt 的智能指针。
     * 
     * @return ClassStmtPtr 解析得到的类声明语句的智能指针。
     */
    ClassStmtPtr classDeclaration();

    /**
     * @brief 解析变量声明语句。
     * 
     * 该函数负责解析变量声明语句，包括变量名、初始值等，并返回一个指向 VarStmt 的智能指针。
     * 
     * @return VarStmtPtr 解析得到的变量声明语句的智能指针。
     */
    VarStmtPtr varDeclaration();

    /**
     * @brief 解析函数声明语句。
     * 
     * 该函数负责解析函数声明语句，根据函数类型（如普通函数、方法等）进行解析，并返回一个指向 FunctionStmt 的智能指针。
     * 
     * @param type 函数的类型，如普通函数、方法等。
     * @return FunctionStmtPtr 解析得到的函数声明语句的智能指针。
     */
    FunctionStmtPtr function(LoxFunctionType type);

    /**
     * @brief 解析代码块语句。
     * 
     * 该函数负责解析由花括号包围的代码块语句，将其中的语句解析并存储在一个语句列表中，并返回该列表。
     * 
     * @return StmtList 解析得到的代码块中的语句列表。
     */
    StmtList block();

    /**
     * @brief 解析 while 循环语句。
     * 
     * 该函数负责解析 while 循环语句，包括循环条件和循环体，并返回一个指向 WhileStmt 的智能指针。
     * 
     * @return WhileStmtPtr 解析得到的 while 循环语句的智能指针。
     */
    WhileStmtPtr whileStatement();

    /**
     * @brief 解析语句，可能是各种类型的语句。
     * 
     * 该函数尝试解析一个语句，并返回解析得到的语句对象。
     * 
     * @return Stmt 解析得到的语句。
     */
    Stmt statements();

    /**
     * @brief 解析通用语句，可能是各种类型的语句。
     * 
     * 该函数尝试解析一个通用语句，并返回解析得到的语句对象。
     * 
     * @return Stmt 解析得到的通用语句。
     */
    Stmt statement();

    /**
     * @brief 解析 for 循环语句。
     * 
     * 该函数负责解析 for 循环语句，包括初始化、条件、迭代和循环体，并返回解析得到的语句对象。
     * 
     * @return Stmt 解析得到的 for 循环语句。
     */
    Stmt forStatement();

    /**
     * @brief 解析 if 条件语句。
     * 
     * 该函数负责解析 if 条件语句，包括条件表达式、then 分支和可选的 else 分支，并返回一个指向 IfStmt 的智能指针。
     * 
     * @return IfStmtPtr 解析得到的 if 条件语句的智能指针。
     */
    IfStmtPtr ifStatement();

    /**
     * @brief 解析 return 语句。
     * 
     * 该函数负责解析 return 语句，包括返回值表达式，并返回一个指向 ReturnStmt 的智能指针。
     * 
     * @return ReturnStmtPtr 解析得到的 return 语句的智能指针。
     */
    ReturnStmtPtr returnStatement();

    /**
     * @brief 解析 print 语句。
     * 
     * 该函数负责解析 print 语句，包括要打印的表达式，并返回一个指向 PrintStmt 的智能指针。
     * 
     * @return PrintStmtPtr 解析得到的 print 语句的智能指针。
     */
    PrintStmtPtr printStatement();

    /**
     * @brief 解析表达式语句。
     * 
     * 该函数负责解析表达式语句，即一个表达式作为一条语句，并返回一个指向 ExpressionStmt 的智能指针。
     * 
     * @return ExpressionStmtPtr 解析得到的表达式语句的智能指针。
     */
    ExpressionStmtPtr expressionStatement();

    /**
     * @brief 解析逻辑与表达式。
     * 
     * 该函数负责解析逻辑与表达式，如 a && b，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的逻辑与表达式。
     */
    Expr and_();

    /**
     * @brief 解析逻辑或表达式。
     * 
     * 该函数负责解析逻辑或表达式，如 a || b，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的逻辑或表达式。
     */
    Expr or_();

    /**
     * @brief 解析赋值表达式。
     * 
     * 该函数负责解析赋值表达式，如 a = b，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的赋值表达式。
     */
    Expr assignment();

    /**
     * @brief 解析逻辑或表达式（另一种实现）。
     * 
     * 该函数负责解析逻辑或表达式，如 a || b，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的逻辑或表达式。
     */
    Expr logicalOr();

    /**
     * @brief 解析逻辑与表达式（另一种实现）。
     * 
     * 该函数负责解析逻辑与表达式，如 a && b，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的逻辑与表达式。
     */
    Expr logicalAnd();

    /**
     * @brief 解析函数调用表达式。
     * 
     * 该函数负责解析函数调用表达式，如 func(a, b)，并返回解析得到的表达式对象。
     * 
     * @return Expr 解析得到的函数调用表达式。
     */
    Expr call();

    /**
     * @brief 完成函数调用表达式的解析。
     * 
     * 该函数在解析函数调用表达式时，处理参数列表和调用操作，并返回解析得到的表达式对象。
     * 
     * @param callee 被调用的函数表达式。
     * @return Expr 完成解析的函数调用表达式。
     */
    Expr finishCall(Expr& callee);


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