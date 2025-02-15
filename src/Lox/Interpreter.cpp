#include "Lox/Environment.h"
#include "Lox/Interpreter.h"
#include "Lox/LoxCallable.h"
#include "Lox/LoxObject.h"
#include "Lox/LoxFunction.h"
#include "Lox/NativeFunction.h"
#include "frontend/Ast.h"
#include <chrono>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <variant>

Interpreter::Interpreter() {
    globals->define(
        "clock", std::make_shared<NativeFunction>(
                     [](const llvm::SmallVector<LoxObject> &) -> LoxObject {
                         const auto now = std::chrono::system_clock::now().time_since_epoch();

                         return static_cast<LoxNumber>(std::chrono::duration_cast<std::chrono::seconds>(now).count());
                     }
                 )
    );
}
Interpreter::~Interpreter() = default;


// LoxObject Interpreter::operator()(const Expr& expr) {
//     // TODO: Implement expression evaluation
//     return LoxObject{};
// }

// StmtResult Interpreter::operator()(const Stmt& stmt) {
//     // TODO: Implement statement evaluation
//     return Nothing{};
// }

/**
 * @brief 处理 FunctionStmt 语句的调用运算符重载。
 * 
 * 该函数执行函数声明语句，将函数名和函数对象存储在当前环境中。
 * 
 * @param functionStmt 指向 FunctionStmt 的智能指针。
 * @return StmtResult 执行结果，通常为 Nothing。
 */
StmtResult Interpreter::operator()(const FunctionStmtPtr &functionStmt){
    // 获取函数名
    const auto name = functionStmt->name.getLexeme();
    // 创建一个 LoxFunction 对象，该对象封装了函数声明和当前环境
    auto function = std::make_shared<LoxFunction>(functionStmt,environment);
    // 在当前环境中定义函数，将函数名和函数对象关联起来
    environment->define(name,std::move(function));
    // 返回 Nothing，表示函数声明语句执行完毕
    return Nothing();
}

/**
 * @brief 处理 IfStmt 语句的调用运算符重载。
 * 
 * 该函数根据 IfStmt 的条件表达式的值，决定执行 then 分支还是 else 分支。
 * 
 * @param ifStmtPtr 指向 IfStmt 的智能指针。
 * @return StmtResult 执行结果。
 */
StmtResult Interpreter::operator()(const IfStmtPtr &ifStmtPtr) {
    // 检查条件表达式是否为真
    if (isTruthy(evaluate(ifStmtPtr->condition))) {
        // 如果条件为真，执行 then 分支
        return std::move(evaluate(ifStmtPtr->thenBranch));
    }

    // 检查是否存在 else 分支
    if (ifStmtPtr->elseBranch.has_value()) {
        // 如果存在 else 分支，执行 else 分支
        return evaluate(ifStmtPtr->elseBranch.value());
    }

    // 如果条件为假且没有 else 分支，返回 Nothing
    return Nothing();
}
StmtResult Interpreter::operator()(const ReturnStmtPtr &returnStmt){
    LoxObject value = LoxNil();
    if(returnStmt->expression.has_value()){
        value = evaluate(returnStmt->expression.value());
    }
    return Return(value);
}
/**
 * @brief 处理 ExpressionStmt 语句的调用运算符重载。
 * 
 * 该函数执行表达式语句，但不返回任何有意义的结果。
 * 
 * @param expressionStmt 指向 ExpressionStmt 的智能指针。
 * @return StmtResult 执行结果，通常为 Nothing。
 */
StmtResult Interpreter::operator()(const ExpressionStmtPtr &expressionStmt) {
    // 执行表达式
    evaluate(expressionStmt->expression);
    // 返回 Nothing
    return Nothing();
}

/**
 * @brief 处理 PrintStmt 语句的调用运算符重载。
 * 
 * 该函数执行打印语句，将表达式的值输出到标准输出。
 * 
 * @param printStmt 指向 PrintStmt 的智能指针。
 * @return StmtResult 执行结果，通常为 Nothing。
 */
StmtResult Interpreter::operator()(const PrintStmtPtr &printStmt) {
    // 计算表达式的值
    const auto object = evaluate(printStmt->expression);
    // 将对象转换为字符串并输出到标准输出
    llvm::outs() << to_string(object) << "\n";
    // 返回 Nothing
    return Nothing();
}

/**
 * @brief 处理 VarStmt 语句的调用运算符重载。
 * 
 * 该函数执行变量声明语句，将变量名和初始值存储在当前环境中。
 * 
 * @param varStmt 指向 VarStmt 的智能指针。
 * @return StmtResult 执行结果，通常为 Nothing。
 */
StmtResult Interpreter::operator()(const VarStmtPtr &varStmt) {
    // 计算初始值
    const auto value = evaluate(varStmt->initializer);
    // 在当前环境中定义变量
    environment->define(varStmt->name.getLexeme(), value);
    // 返回 Nothing
    return Nothing();
}

/**
 * @brief 处理 WhileStmt 语句的调用运算符重载。
 * 
 * 该函数执行 while 循环语句，只要条件为真就继续执行循环体。
 * 
 * @param whileStmt 指向 WhileStmt 的智能指针。
 * @return StmtResult 执行结果，如果循环体中遇到 Return 语句则返回该结果，否则返回 Nothing。
 */
StmtResult Interpreter::operator()(const WhileStmtPtr &whileStmt) {
    // 只要条件为真，就继续执行循环体
    while (isTruthy(evaluate(whileStmt->condition))) {
        // 执行循环体
        if (auto result = evaluate(whileStmt->body); std::holds_alternative<Return>(result)) {
            // 如果循环体中遇到 Return 语句，返回该结果
            return result;
        }
    }
    // 如果没有遇到 Return 语句，返回 Nothing
    return Nothing();
}

/**
 * @brief 处理函数调用表达式的调用运算符重载。
 * 
 * 该函数用于计算并执行函数调用表达式。它会检查调用深度是否超过限制，
 * 计算被调用函数的表达式值，收集参数，并确保参数数量与函数期望的参数数量匹配。
 * 如果一切正常，它会调用函数并返回结果；否则，抛出运行时错误。
 * 
 * @param callExpr 指向 CallExpr 的智能指针，表示函数调用表达式。
 * @return LoxObject 函数调用的结果。
 */
LoxObject Interpreter::operator()(const CallExprPtr &callExpr) {
    // 检查函数调用深度是否超过最大限制
    if (function_depth > MAX_CALL_DEPTH) {
        // 如果超过限制，抛出运行时错误
        throw runtime_error(callExpr->keyword, "Stack overflow.");
    }

    // 计算被调用函数的表达式的值
    const auto &callee = evaluate(callExpr->callee);

    // 用于存储函数调用的参数
    llvm::SmallVector<LoxObject> arguments;
    // 遍历调用表达式中的参数列表
    for (auto &argument: callExpr->arguments) {
        // 计算每个参数的值并添加到参数列表中
        arguments.push_back(evaluate(argument));
    }

    // 检查被调用的对象是否为可调用对象
    if (std::holds_alternative<LoxCallablePtr>(callee)) {
        // 获取可调用对象
        const auto &callable = std::get<LoxCallablePtr>(callee);
        // 检查传递的参数数量是否与可调用对象期望的参数数量一致
        if (static_cast<int>(arguments.size()) != callable->arity()) {
            // 构建错误信息，说明期望的参数数量和实际传递的参数数量
            std::string result = "Expected " + std::to_string(callable->arity()) + " arguments but got " +
                                 std::to_string(arguments.size()) + ".";
            // 抛出运行时错误，包含错误信息
            throw runtime_error(callExpr->keyword, result);
        }
        // 增加函数调用深度
        function_depth++;
        // 调用可调用对象并传递解释器和参数列表，获取返回值
        auto lox_object = (*callable)(*this, arguments);
        // 减少函数调用深度
        function_depth--;
        // 返回函数调用的结果
        return lox_object;
    }

    // 如果被调用的对象不是可调用对象，抛出运行时错误
    throw runtime_error(callExpr->keyword, "Can only call functions and classes.");
}

/**
 * @brief 计算表达式的值。
 * 
 * 该函数使用 std::visit 来调用 Interpreter 类的相应重载运算符，以计算表达式的值。
 * 
 * @param expr 要计算的表达式。
 * @return LoxObject 表达式的值。
 */
LoxObject Interpreter::evaluate(const Expr &expr) { return std::visit(*this, expr); }

/**
 * @brief 执行语句。
 * 
 * 该函数使用 std::visit 来调用 Interpreter 类的相应重载运算符，以执行语句。
 * 
 * @param stmt 要执行的语句。
 * @return StmtResult 执行结果。
 */
StmtResult Interpreter::evaluate(const Stmt &stmt) { return std::visit(*this, stmt); }

/**
 * @brief 执行程序。
 * 
 * 该函数依次执行程序中的每个语句，并捕获可能的运行时错误。
 * 
 * @param program 要执行的程序，即语句列表。
 */
void Interpreter::evaluate(const Program &program) {
    try {
        // 依次执行程序中的每个语句
        for (const auto &stmt: program) { evaluate(stmt); }
    } catch (const runtime_error &e) {
        // 捕获并处理运行时错误
        runtimeError(e);
    }
}

/**
 * @brief 执行代码块。
 * 
 * 该函数在新的环境中执行代码块，并在执行完毕后恢复原来的环境。
 * 
 * @param statements 要执行的语句列表。
 * @param newenvironment 新的环境。
 * @return StmtResult 执行结果，如果代码块中遇到 Return 语句则返回该结果，否则返回 Nothing。
 */
StmtResult Interpreter::executeblock(const StmtList &statements, const EnvironmentPtr &newenvironment) {
    // 保存原来的环境
    const auto previous = this->environment;
    // 设置新的环境
    environment = newenvironment;

    // 依次执行代码块中的每个语句
    for (const auto &statement: statements) {
        // 执行语句
        if (auto result = evaluate(statement); !std::holds_alternative<Nothing>(result)) {
            // 如果语句执行结果不是 Nothing，恢复原来的环境并返回该结果
            environment = previous;
            return result;
        }
    }

    // 恢复原来的环境
    environment = previous;
    // 如果没有遇到 Return 语句，返回 Nothing
    return Nothing{};
}
