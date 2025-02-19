#pragma once
// 引入工具类的头文件，提供一些通用的工具函数和类型定义
#include "Utils/Utils.h"
// 引入前端词法单元的头文件，定义了词法单元的类型和结构
#include "frontend/Token.h"
// 引入智能指针相关的头文件，用于管理动态分配的内存
#include <llvm/ADT/SmallVector.h>
#include <memory>
#include <vector>

/**
 * @brief 定义一元操作符的枚举类型。
 * 
 * 该枚举类型将一元操作符与词法单元类型进行关联，方便在解析和处理表达式时使用。
 */
enum class UnaryOp {
    // 逻辑非操作符，对应词法单元类型中的 BANG
    BANG = TokenType::BANG,
    // 负号操作符，对应词法单元类型中的 MINUS
    MINUS = TokenType::MINUS
};

/**
 * @brief 定义二元操作符的枚举类型。
 * 
 * 该枚举类型将二元操作符与词法单元类型进行关联，涵盖了常见的算术、比较和相等性操作符。
 */
enum class BinaryOp {
    // 加法操作符，对应词法单元类型中的 PLUS
    PLUS = TokenType::PLUS,
    // 减法操作符，对应词法单元类型中的 MINUS
    MINUS = TokenType::MINUS,
    // 除法操作符，对应词法单元类型中的 SLASH
    SLASH = TokenType::SLASH,
    // 乘法操作符，对应词法单元类型中的 STAR
    STAR = TokenType::STAR,
    // 大于操作符，对应词法单元类型中的 GREATER
    GREATER = TokenType::GREATER,
    // 大于等于操作符，对应词法单元类型中的 GREATER_EQUAL
    GREATER_EQUAL = TokenType::GREATER_EQUAL,
    // 小于操作符，对应词法单元类型中的 LESS
    LESS = TokenType::LESS,
    // 小于等于操作符，对应词法单元类型中的 LESS_EQUAL
    LESS_EQUAL = TokenType::LESS_EQUAL,
    // 不等于操作符，对应词法单元类型中的 BANG_EQUAL
    BANG_EQUAL = TokenType::BANG_EQUAL,
    // 等于操作符，对应词法单元类型中的 EQUAL_EQUAL
    EQUAL_EQUAL = TokenType::EQUAL_EQUAL
};

/**
 * @brief 定义逻辑操作符的枚举类型。
 * 
 * 该枚举类型将逻辑操作符与词法单元类型进行关联，用于处理逻辑表达式。
 */
enum class LogicalOp {
    // 逻辑或操作符，对应词法单元类型中的 OR
    OR = TokenType::OR,
    // 逻辑与操作符，对应词法单元类型中的 AND
    AND = TokenType::AND
};

/**
 * @brief 定义 Lox 语言函数类型的枚举类型。
 * 
 * 该枚举类型用于区分不同类型的 Lox 函数，如普通函数、构造函数和方法。
 */
enum class LoxFunctionType {
    // 无函数类型
    NONE,
    // 普通函数
    FUNCTION,
    // 构造函数
    INITIALIZER,
    // 方法
    METHOD
};

// 前向声明各种表达式结构体，以便在后续代码中使用指针类型
class BinaryExpr;
class CallExpr;
class GetExpr;
class SetExpr;
class ThisExpr;
class SuperExpr;
class GroupingExpr;
class LiteralExpr;
class LogicalExpr;
class UnaryExpr;
class VarExpr;
class AssignExpr;

// 定义各种表达式结构体的智能指针类型，使用 std::unique_ptr 管理内存
using BinaryExprPtr = std::unique_ptr<BinaryExpr>;
using CallExprPtr = std::unique_ptr<CallExpr>;
using GetExprPtr = std::unique_ptr<GetExpr>;
using SetExprPtr = std::unique_ptr<SetExpr>;
using ThisExprPtr = std::unique_ptr<ThisExpr>;
using SuperExprPtr = std::unique_ptr<SuperExpr>;
using GroupingExprPtr = std::unique_ptr<GroupingExpr>;
using LiteralExprPtr = std::unique_ptr<LiteralExpr>;
using LogicalExprPtr = std::unique_ptr<LogicalExpr>;
using UnaryExprPtr = std::unique_ptr<UnaryExpr>;
using VarExprPtr = std::unique_ptr<VarExpr>;
using AssignExprPtr = std::unique_ptr<AssignExpr>;

/**
 * @brief 定义表达式的变体类型。
 * 
 * 该变体类型可以存储各种表达式结构体的智能指针，方便在代码中统一处理不同类型的表达式。
 */
using Expr = std::variant<
    BinaryExprPtr, CallExprPtr, GetExprPtr, SetExprPtr, ThisExprPtr, SuperExprPtr, GroupingExprPtr, LiteralExprPtr,
    LogicalExprPtr, UnaryExprPtr, VarExprPtr, AssignExprPtr>;

/**
 * @brief 二元表达式类。
 * 
 * 该类表示一个二元表达式，由左操作数、操作符和右操作数组成。
 * 继承自 Uncopyable 类，确保对象不可复制。
 */
class BinaryExpr : Uncopyable {
public:
    /**
     * @brief 构造函数，初始化二元表达式。
     * 
     * @param left 左操作数表达式。
     * @param op 操作符词法单元。
     * @param right 右操作数表达式。
     */
    explicit BinaryExpr(Expr left, const Token &token, const BinaryOp op, Expr right)
        : left(std::move(left)), token{token}, op{op}, right{std::move(right)} {}
    Expr left;
    Token token;
    BinaryOp op;
    Expr right;
};

/**
 * @brief 调用表达式类，表示函数或方法的调用操作。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含被调用的表达式、调用的关键字和参数列表。
 */
class CallExpr : Uncopyable {
public:
    // 被调用的表达式，通常是一个函数或方法
    Expr callee;
    // 调用的关键字，例如函数名对应的词法单元
    Token keyword;
    // 调用时传递的参数列表
    std::vector<Expr> arguments;

    /**
     * @brief 构造函数，初始化调用表达式。
     * 
     * @param callee 被调用的表达式。
     * @param keyword 调用的关键字。
     * @param arguments 调用时传递的参数列表。
     */
    explicit CallExpr(Expr callee, const Token &keyword, std::vector<Expr> arguments)
        : callee{std::move(callee)}, keyword{keyword}, arguments{std::move(arguments)} {}
};


/**
 * @brief 一元表达式类。
 * 
 * 该类表示一个一元表达式，由操作符和右操作数组成。
 * 继承自 Uncopyable 类，确保对象不可复制。
 */
class UnaryExpr : Uncopyable {
public:
    /**
     * @brief 构造函数，初始化一元表达式。
     * 
     * @param op 操作符词法单元。
     * @param right 右操作数表达式。
     */
   explicit UnaryExpr(const Token &token, const UnaryOp op, Expr expression)
            : token{token}, op{op}, expression{std::move(expression)} {}
    Token token;
    UnaryOp op;
    Expr expression;
};

/**
 * @brief 字面量表达式类。
 * 
 * 该类表示一个字面量表达式，包含一个字面量值。
 * 继承自 Uncopyable 类，确保对象不可复制。
 */
class LiteralExpr : Uncopyable {
public:
    // 字面量值
    Literal value;


    /**
     * @brief 构造函数，初始化字面量表达式。
     * 
     * @param value 字面量值。
     */
    explicit LiteralExpr(Literal value) : value(value) {}
};

/**
 * @brief 分组表达式类。
 * 
 * 该类表示一个分组表达式，通常由括号括起来的表达式。
 * 继承自 Uncopyable 类，确保对象不可复制。
 */
class GroupingExpr : Uncopyable {
public:
    // 分组内的表达式
    Expr expression;


    /**
     * @brief 构造函数，初始化分组表达式。
     * 
     * @param expression 分组内的表达式。
     */
    explicit GroupingExpr(Expr expression) : expression(std::move(expression)) {}
};

/**
 * @brief 可赋值对象类。
 * 
 * 该类表示一个可赋值的对象，包含一个名称词法单元，以及一些用于作用域分析的元数据。
 * 继承自 Uncopyable 类，确保对象不可复制。
 */
class Assignable : Uncopyable {
public:
    // 可赋值对象的名称词法单元
    Token name;
    // 作用域距离，用于作用域分析
    mutable signed long distance = -1;
    // 是否被捕获的标志，用于闭包分析
    mutable bool isCaptured = false;


    /**
     * @brief 构造函数，初始化可赋值对象。
     * 
     * @param name 可赋值对象的名称词法单元。
     */
    explicit Assignable(const Token &name) : name(name) {}
};
/**
 * @brief 获取表达式类，表示对对象属性的获取操作。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含一个对象表达式和一个属性名的词法单元。
 */
class GetExpr : private Uncopyable {
public:
    // 要获取属性的对象表达式
    Expr object;
    // 属性名的词法单元
    Token name;


    /**
     * @brief 构造函数，初始化获取表达式。
     * 
     * @param object 要获取属性的对象表达式。
     * @param name 属性名的词法单元。
     */
    explicit GetExpr(Expr object, const Token &name) : object{std::move(object)}, name{name} {}
};

/**
 * @brief 设置表达式类，表示对对象属性的赋值操作。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含一个对象表达式、一个属性名的词法单元和一个赋值的值表达式。
 */
class SetExpr : Uncopyable {
public:
    // 要设置属性的对象表达式
    Expr object;
    // 属性名的词法单元
    Token name;
    // 要赋值的值表达式
    Expr value;


    /**
     * @brief 构造函数，初始化设置表达式。
     * 
     * @param object 要设置属性的对象表达式。
     * @param name 属性名的词法单元。
     * @param value 要赋值的值表达式。
     */
    explicit SetExpr(Expr object, const Token &name, Expr value)
        : object{std::move(object)}, name{name}, value{std::move(value)} {}
};

/**
 * @brief This 表达式类，表示当前对象的引用。
 * 
 * 该类继承自 Assignable，包含一个名称词法单元，用于表示当前对象。
 */
class ThisExpr : public Assignable {
public:
    /**
     * @brief 构造函数，初始化 This 表达式。
     * 
     * @param name 表示当前对象的名称词法单元。
     */
    explicit ThisExpr(const Token &name) : Assignable(name) {}
};

/**
 * @brief Super 表达式类，表示对父类方法或属性的引用。
 * 
 * 该类继承自 Assignable，包含一个名称词法单元和一个方法名的词法单元。
 */
class SuperExpr : public Assignable {
public:
    // 要调用的父类方法名的词法单元
    Token method;


    /**
     * @brief 构造函数，初始化 Super 表达式。
     * 
     * @param name 表示父类的名称词法单元。
     * @param method 要调用的父类方法名的词法单元。
     */
    explicit SuperExpr(const Token &name, const Token &method) : Assignable(name), method{method} {}
};

/**
 * @brief 逻辑表达式类，表示逻辑运算表达式。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含左操作数表达式、逻辑操作符和右操作数表达式。
 */
class LogicalExpr : Uncopyable {
public:
    // 左操作数表达式
    Expr left;
    // 逻辑操作符
    LogicalOp op;
    // 右操作数表达式
    Expr right;


    /**
     * @brief 构造函数，初始化逻辑表达式。
     * 
     * @param left 左操作数表达式。
     * @param op 逻辑操作符。
     * @param right 右操作数表达式。
     */
    explicit LogicalExpr(Expr left, const LogicalOp op, Expr right)
        : left{std::move(left)}, op{op}, right{std::move(right)} {}
};

/**
 * @brief 变量表达式类，表示对变量的引用。
 * 
 * 该类继承自 Assignable，包含一个名称词法单元，用于表示变量。
 */
class VarExpr : public Assignable {
public:
    /**
     * @brief 构造函数，初始化变量表达式。
     * 
     * @param name 表示变量的名称词法单元。
     */
    explicit VarExpr(const Token &name) : Assignable(name) {}
};

/**
 * @brief 赋值表达式类，表示对变量的赋值操作。
 * 
 * 该类继承自 Assignable，包含一个名称词法单元和一个赋值的值表达式。
 */
class AssignExpr : public Assignable {
public:
    // 要赋值的值表达式
    Expr value;


    /**
     * @brief 构造函数，初始化赋值表达式。
     * 
     * @param name 表示变量的名称词法单元。
     * @param value 要赋值的值表达式。
     */
    AssignExpr(const Token &name, Expr value) : Assignable(name), value{std::move(value)} {}
};

// 前向声明各种语句类，以便在后续代码中使用指针类型
class ExpressionStmt;
class FunctionStmt;
class ReturnStmt;
class IfStmt;
class PrintStmt;
class VarStmt;
class BlockStmt;
class WhileStmt;
class ClassStmt;

// 定义各种语句类的智能指针类型，使用 std::shared_ptr 管理内存
using ExpressionStmtPtr = std::shared_ptr<ExpressionStmt>;
using FunctionStmtPtr = std::shared_ptr<FunctionStmt>;
using ReturnStmtPtr = std::shared_ptr<ReturnStmt>;
using IfStmtPtr = std::shared_ptr<IfStmt>;
using PrintStmtPtr = std::shared_ptr<PrintStmt>;
using VarStmtPtr = std::shared_ptr<VarStmt>;
using BlockStmtPtr = std::shared_ptr<BlockStmt>;
using WhileStmtPtr = std::shared_ptr<WhileStmt>;
using ClassStmtPtr = std::shared_ptr<ClassStmt>;

/**
 * @brief 定义语句的变体类型。
 * 
 * 该变体类型可以存储各种语句类的智能指针，方便在代码中统一处理不同类型的语句。
 */
using Stmt = std::variant<
    ExpressionStmtPtr, FunctionStmtPtr, ReturnStmtPtr, IfStmtPtr, PrintStmtPtr, VarStmtPtr, BlockStmtPtr, WhileStmtPtr,
    ClassStmtPtr>;

/**
 * @brief 定义语句列表类型。
 * 
 * 该类型使用 std::vector 存储语句变体类型，用于表示一组语句。
 */
using StmtList = std::vector<Stmt>;

/**
 * @brief 表达式语句类，表示一个表达式作为语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含一个表达式。
 */
class ExpressionStmt : Uncopyable {
public:
    // 表达式
    Expr expression;


    /**
     * @brief 构造函数，初始化表达式语句。
     * 
     * @param expression 表达式。
     */
    explicit ExpressionStmt(Expr expression) : expression{std::move(expression)} {}
};

/**
 * @brief If 语句类，表示条件判断语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含一个条件表达式、一个 then 分支语句和一个可选的 else 分支语句。
 */
class IfStmt : Uncopyable {
public:
    // 条件表达式
    Expr condition;
    // then 分支语句
    Stmt thenBranch;
    // 可选的 else 分支语句
    std::optional<Stmt> elseBranch;
    /**
     * @brief 构造函数，初始化 If 语句。
     * 
     * @param condition 条件表达式。
     * @param thenBranch then 分支语句。
     * @param elseBranch 可选的 else 分支语句。
     */

    explicit IfStmt(Expr condition, Stmt thenBranch, std::optional<Stmt> elseBranch)
        : condition{std::move(condition)}, thenBranch{std::move(thenBranch)}, elseBranch{std::move(elseBranch)} {}
};

/**
 * @brief 函数语句类，表示函数定义语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含函数名的词法单元、函数类型、参数列表和函数体语句列表。
 */
class FunctionStmt : Uncopyable {
public:
    // 函数名的词法单元
    Token name;
    // 函数类型
    LoxFunctionType type;
    // 参数列表
    std::vector<Token> parameters;
    // 函数体语句列表
    StmtList body;


    /**
     * @brief 构造函数，初始化函数语句。
     * 
     * @param name 函数名的词法单元。
     * @param type 函数类型。
     * @param parameters 参数列表。
     * @param body 函数体语句列表。
     */
    explicit FunctionStmt(const Token &name, const LoxFunctionType type, std::vector<Token> parameters, StmtList body)
        : name{name}, type{type}, parameters{std::move(parameters)}, body{std::move(body)} {}
};

/**
 * @brief 返回语句类，表示函数中的返回语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含返回关键字的词法单元和可选的返回表达式。
 */
class ReturnStmt : Uncopyable {
public:
    // 返回关键字的词法单元
    Token keyword;
    // 可选的返回表达式
    std::optional<Expr> expression;
    /**
     * @brief 构造函数，初始化返回语句。
     * 
     * @param keyword 返回关键字的词法单元。
     * @param expression 可选的返回表达式。
     */

    explicit ReturnStmt(const Token &keyword, std::optional<Expr> expression)
        : keyword{keyword}, expression{std::move(expression)} {}
};

/**
 * @brief 打印语句类，表示打印表达式结果的语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含要打印的表达式。
 */
class PrintStmt : Uncopyable {
public:
    // 要打印的表达式
    Expr expression;
    /**
     * @brief 构造函数，初始化打印语句。
     * 
     * @param expression 要打印的表达式。
     */

    explicit PrintStmt(Expr expression) : expression{std::move(expression)} {}
};

/**
 * @brief 变量声明语句类，表示变量的声明和初始化。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含变量名的词法单元和初始化表达式。
 */
class VarStmt : Uncopyable {
public:
    // 变量名的词法单元
    Token name;
    // 初始化表达式
    Expr initializer;


    /**
     * @brief 构造函数，初始化变量声明语句。
     * 
     * @param name 变量名的词法单元。
     * @param initializer 初始化表达式。
     */
    explicit VarStmt(const Token &name, Expr initializer) : name{name}, initializer{std::move(initializer)} {}
};

/**
 * @brief 代码块语句类，表示由一组语句组成的代码块。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含代码块内的语句列表。
 */
class BlockStmt : Uncopyable {
public:
    // 代码块内的语句列表
    StmtList statements;


    /**
     * @brief 构造函数，初始化代码块语句。
     * 
     * @param statements 代码块内的语句列表。
     */
    explicit BlockStmt(StmtList statements) : statements{std::move(statements)} {}
};

/**
 * @brief 循环语句类，表示 while 循环语句。
 * 
 * 该类继承自 Uncopyable，确保对象不可复制。它包含循环条件表达式和循环体语句。
 */
class WhileStmt : Uncopyable {
public:
    // 循环条件表达式
    Expr condition;
    // 循环体语句
    Stmt body;


    /**
     * @brief 构造函数，初始化 while 循环语句。
     * 
     * @param condition 循环条件表达式。
     * @param body 循环体语句。
     */
    WhileStmt(Expr condition, Stmt body) : condition{std::move(condition)}, body{std::move(body)} {}
};

/**
 * @brief 类声明语句类，表示类的定义。
 * 
 * 该类包含类名的词法单元、可选的父类变量表达式和类方法列表。
 */
class ClassStmt {
public:
    // 类名的词法单元
    Token name;
    // 可选的父类变量表达式
    std::optional<VarExprPtr> super_class;
    // 类方法列表
    std::vector<FunctionStmtPtr> methods;


    /**
     * @brief 构造函数，初始化类声明语句。
     * 
     * @param name 类名的词法单元。
     * @param super_class 可选的父类变量表达式。
     * @param methods 类方法列表。
     */
    ClassStmt(const Token &name, std::optional<VarExprPtr> super_class, std::vector<FunctionStmtPtr> methods)
        : name{name}, super_class{std::move(super_class)}, methods{std::move(methods)} {}
};

/**
 * @brief 程序类型定义，表示一个程序由一组语句组成。
 * 
 * 使用 std::vector 存储语句变体类型。
 */
using Program = std::vector<Stmt>;