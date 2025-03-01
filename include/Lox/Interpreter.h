#pragma once

#include "Lox/Environment.h"
#include "Lox/LoxObject.h"
#include "frontend/Ast.h"
#include <memory>

static int  MAX_CALL_DEPTH=100;
struct Return {
    LoxObject &value;
    ~Return() = default;
    // explicit Return(LoxObject &value) : value(value) {};
};

struct Nothing {};
using StmtResult = std::variant<LoxObject, Return, Nothing>;
class Interpreter {
public:
    Interpreter();
    ~Interpreter()=default;
    //void interpret(const std::vector<StmtPtr> &statements);
    StmtResult operator()(const ExpressionStmtPtr &expressionStmt);
    StmtResult operator()(const IfStmtPtr &ifStmtPtr);
    StmtResult operator()(const PrintStmtPtr &printStmt);
    StmtResult operator()(const VarStmtPtr &varStmt);
    StmtResult operator()(const FunctionStmtPtr &functionStmt);
    StmtResult operator()(const ReturnStmtPtr &returnStmt);
    StmtResult operator()(const BlockStmtPtr &blockStmt);
    StmtResult operator()(const WhileStmtPtr &whileStmt);
    StmtResult operator()(const ClassStmtPtr &classStmt);
    LoxObject operator()(const BinaryExprPtr &binaryExpr);
    LoxObject operator()(const CallExprPtr &callExpr);
    LoxObject operator()(const GetExprPtr &getExpr);
    LoxObject operator()(const SetExprPtr &setExpr);
    LoxObject operator()(const ThisExprPtr &thisExpr) const;
    LoxObject operator()(const SuperExprPtr &superExpr) const;
    LoxObject operator()(const GroupingExprPtr &groupingExpr);
    LoxObject operator()(const LiteralExprPtr &literalExpr) const;
    LoxObject operator()(const LogicalExprPtr &logicalExpr);
    LoxObject operator()(const UnaryExprPtr &unaryExpr);
    LoxObject operator()(const VarExprPtr &varExpr) const;
    LoxObject operator()(const AssignExprPtr &assignExpr);

       /**
     * @brief 计算表达式的值
     * 
     * 该函数接收一个表达式对象，对其进行求值并返回结果。
     * 
     * @param expr 要计算的表达式对象
     * @return LoxObject 表达式计算结果
     */
    LoxObject evaluate(const Expr &expr);

    /**
     * @brief 计算语句的执行结果
     * 
     * 该函数接收一个语句对象，执行该语句并返回执行结果。
     * 
     * @param stmt 要执行的语句对象
     * @return StmtResult 语句执行结果
     */
    StmtResult evaluate(const Stmt &stmt);

    /**
     * @brief 执行整个程序
     * 
     * 该函数接收一个程序对象，执行程序中的所有语句。
     * 
     * @param program 要执行的程序对象
     */
    void evaluate(const Program &program);

    /**
     * @brief 在新环境中执行语句块
     * 
     * 该函数在指定的新环境中执行一组语句，并返回执行结果。
     * 
     * @param statements 要执行的语句列表
     * @param newenvironment 执行语句的新环境
     * @return StmtResult 语句块执行结果
     */
    StmtResult executeBlock(const StmtList &statements, const EnvironmentPtr &newenvironment);

private:
    // 全局环境指针，初始化为一个新的环境
    EnvironmentPtr globals = std::make_shared<Environment>();
    // 当前环境指针，初始指向全局环境
    EnvironmentPtr environment = globals;
    // 函数调用深度计数器
    int function_depth = 0;

    // std::unordered_map<std::string_view, LoxFunctionPtr> methods;
    /**
     * @brief 检查操作数是否为数字类型
     * 
     * 该函数检查给定的操作数是否为 LoxNumber 类型，如果是则返回该操作数，否则抛出运行时错误。
     * 
     * @param op 操作符的 Token 对象
     * @param operand 要检查的操作数
     * @return LoxNumber 如果操作数是 LoxNumber 类型，则返回该操作数
     * @throws runtime_error 如果操作数不是 LoxNumber 类型
     */
    static LoxNumber checkNumberOperand(const Token &op, const LoxObject &operand) {
        // 检查操作数是否为 LoxNumber 类型
        if (std::holds_alternative<LoxNumber>(operand)) { return std::get<LoxNumber>(operand);
}
        // 若不是 LoxNumber 类型，抛出运行时错误
        throw runtime_error(op, "Operand must be a number.");
    }

    /**
     * @brief 检查操作数对是否都为数字类型
     * 
     * 该函数检查给定的左右操作数是否都为 LoxNumber 类型，如果是则直接返回，否则抛出运行时错误。
     * 
     * @param op 操作符的 Token 对象
     * @param left 左操作数
     * @param right 右操作数
     * @throws runtime_error 如果左右操作数不全是 LoxNumber 类型
     */
    static void checkNumberOperands(const Token &op, const LoxObject &left, const LoxObject &right) {
        // 检查左右操作数是否都为 LoxNumber 类型
        if (std::holds_alternative<LoxNumber>(left) && std::holds_alternative<LoxNumber>(right)) { return;
}
        // 若不全是 LoxNumber 类型，抛出运行时错误
        throw runtime_error(op, "Operands must be numbers.");

    }

        /**
     * @brief 查找变量的值
     * 
     * 该函数根据给定的 Token 和可赋值表达式，查找并返回变量的值。
     * 它会在当前解释器的环境中进行查找，考虑作用域和变量的绑定情况。
     * 
     * @param name 变量的 Token 对象，包含变量的名称和位置信息
     * @param expr 可赋值表达式，可能包含变量的作用域信息
     * @return LoxObject& 返回找到的变量的值的引用
     */
    [[nodiscard]] LoxObject &lookUpVariable(const Token &name, const Assignable &expr) const;

};