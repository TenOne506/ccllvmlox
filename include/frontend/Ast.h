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
    BinaryExpr(Expr left, Token op, Expr right) : left(std::move(left)), op(op), right(std::move(right)) {}
    // 左操作数表达式
    Expr left;
    // 操作符词法单元
    Token op;
    // 右操作数表达式
    Expr right;
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
    UnaryExpr(Token op, Expr right) : op(op), right(std::move(right)) {}
    // 操作符词法单元
    Token op;
    // 右操作数表达式
    Expr right;
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
class GetExpr : private Uncopyable {
    Expr object;
    Token name;
    explicit GetExpr(Expr object, const Token &name) : object{std::move(object)}, name{name} {}
};

class SetExpr : Uncopyable {
    Expr object;
    Token name;
    Expr value;
    explicit SetExpr(Expr object, const Token &name, Expr value)
        : object{std::move(object)}, name{name}, value{std::move(value)} {}
};

class ThisExpr : Assignable {
    explicit ThisExpr(const Token &name) : Assignable(name) {}
};

class SuperExpr : Assignable {
    Token method;
    explicit SuperExpr(const Token &name, const Token &method) : Assignable(name), method{method} {}
};


class LogicalExpr : Uncopyable {
    Expr left;
    LogicalOp op;
    Expr right;
    explicit LogicalExpr(Expr left, const LogicalOp op, Expr right)
        : left{std::move(left)}, op{op}, right{std::move(right)} {}
};

class VarExpr : Assignable {
public:
    explicit VarExpr(const Token &name) : Assignable(name) {}
};

class AssignExpr : Assignable {
    Expr value;

public:
    AssignExpr(const Token &name, Expr value) : Assignable(name), value{std::move(value)} {}
};
class ExpressionStmt;
class FunctionStmt;
class ReturnStmt;
class IfStmt;
class PrintStmt;
class VarStmt;
class BlockStmt;
class WhileStmt;
class ClassStmt;

using ExpressionStmtPtr = std::shared_ptr<ExpressionStmt>;
using FunctionStmtPtr = std::shared_ptr<FunctionStmt>;
using ReturnStmtPtr = std::shared_ptr<ReturnStmt>;
using IfStmtPtr = std::shared_ptr<IfStmt>;
using PrintStmtPtr = std::shared_ptr<PrintStmt>;
using VarStmtPtr = std::shared_ptr<VarStmt>;
using BlockStmtPtr = std::shared_ptr<BlockStmt>;
using WhileStmtPtr = std::shared_ptr<WhileStmt>;
using ClassStmtPtr = std::shared_ptr<ClassStmt>;

using Stmt = std::variant<
    ExpressionStmtPtr, FunctionStmtPtr, ReturnStmtPtr, IfStmtPtr, PrintStmtPtr, VarStmtPtr, BlockStmtPtr, WhileStmtPtr,
    ClassStmtPtr>;

using StmtList = llvm::SmallVector<Stmt>;

class ExpressionStmt : Uncopyable {
    Expr expression;
    explicit ExpressionStmt(Expr expression) : expression{std::move(expression)} {}
};

class IfStmt : Uncopyable {
    Expr condition;
    Stmt thenBranch;
    std::optional<Stmt> elseBranch;
    explicit IfStmt(Expr condition, Stmt thenBranch, std::optional<Stmt> elseBranch)
        : condition{std::move(condition)}, thenBranch{std::move(thenBranch)}, elseBranch{std::move(elseBranch)} {}
};

class FunctionStmt : Uncopyable {
    Token name;
    LoxFunctionType type;
    llvm::SmallVector<Token> parameters;
    StmtList body;

public:
    explicit FunctionStmt(const Token &name, const LoxFunctionType type, llvm::SmallVector<Token> parameters, StmtList body)
        : name{name}, type{type}, parameters{std::move(parameters)}, body{std::move(body)} {}
};

class ReturnStmt : Uncopyable {
    Token keyword;
    std::optional<Expr> expression;
    explicit ReturnStmt(const Token &keyword, std::optional<Expr> expression)
        : keyword{keyword}, expression{std::move(expression)} {}
};

class PrintStmt : Uncopyable {
    Expr expression;
    explicit PrintStmt(Expr expression) : expression{std::move(expression)} {}
};

class VarStmt : Uncopyable {
    Token name;
    Expr initializer;

public:
    explicit VarStmt(const Token &name, Expr initializer) : name{name}, initializer{std::move(initializer)} {}
};

class BlockStmt : Uncopyable {
    StmtList statements;

public:
    explicit BlockStmt(StmtList statements) : statements{std::move(statements)} {}
};

class WhileStmt : Uncopyable {
    Expr condition;
    Stmt body;

public:
    WhileStmt(Expr condition, Stmt body) : condition{std::move(condition)}, body{std::move(body)} {}
};

class ClassStmt {
    Token name;
    std::optional<VarExprPtr> super_class;
    llvm::SmallVector<FunctionStmtPtr> methods;

public:
    ClassStmt(const Token &name, std::optional<VarExprPtr> super_class, llvm::SmallVector<FunctionStmtPtr> methods)
        : name{name}, super_class{std::move(super_class)}, methods{std::move(methods)} {}
};

using Program = llvm::SmallVector<Stmt>;