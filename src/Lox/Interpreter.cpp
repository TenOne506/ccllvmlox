#include "Lox/Interpreter.h"
#include "Lox/Environment.h"
#include "Lox/LoxCallable.h"
#include "Lox/LoxClass.h"
#include "Lox/LoxFunction.h"
#include "Lox/LoxInstance.h"
#include "Lox/LoxObject.h"
#include "Lox/NativeFunction.h"
#include "frontend/Ast.h"
#include <chrono>
#include <iostream>
#include <llvm/ADT/SmallVector.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>
#include <ostream>
#include <variant>

Interpreter::Interpreter() {
    globals->define("clock", std::make_shared<NativeFunction>([](const std::vector<LoxObject> &) -> LoxObject {
                        const auto now = std::chrono::system_clock::now().time_since_epoch();
                        return LoxNumber(std::chrono::duration_cast<std::chrono::seconds>(now).count());
                    }));
}


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
StmtResult Interpreter::operator()(const FunctionStmtPtr &functionStmt) {
    // 获取函数名
    const auto name = functionStmt->name.getLexeme();
    // 创建一个 LoxFunction 对象，该对象封装了函数声明和当前环境
    auto function = std::make_shared<LoxFunction>(functionStmt, environment);
    // 在当前环境中定义函数，将函数名和函数对象关联起来
    environment->define(name, std::move(function));
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

// StmtResult Interpreter::operator()(const IfStmtPtr &ifStmtPtr) {
//         if (isTruthy(evaluate(ifStmtPtr->condition))) { return std::move(evaluate(ifStmtPtr->thenBranch)); }

//         if (ifStmtPtr->elseBranch.has_value()) { return evaluate(ifStmtPtr->elseBranch.value()); }

//         return Nothing();
//     }
/**
 * @brief 处理 ReturnStmt 语句的调用运算符重载。
 *
 * 该函数执行返回语句，根据返回语句中是否包含表达式来确定返回值。
 * 如果包含表达式，则计算表达式的值并将其作为返回值；如果不包含表达式，则返回 LoxNil。
 *
 * @param returnStmt 指向 ReturnStmt 的智能指针。
 * @return StmtResult 执行结果，包含返回值的 Return 对象。
 */
StmtResult Interpreter::operator()(const ReturnStmtPtr &returnStmt) {
    // 初始化返回值为 LoxNil
    LoxObject value = LoxNil();
    // 检查返回语句中是否包含表达式
    if (returnStmt->expression.has_value()) {
        // 如果包含表达式，计算表达式的值并赋值给返回值
        value = evaluate(returnStmt->expression.value());
    }
    // 返回包含返回值的 Return 对象
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
    //llvm::outs() << to_string(object) << "\n";
    //auto temp = to_string(object);

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
    std::vector<LoxObject> arguments;
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
            // std::string result = "Expected " + std::to_string(callable->arity()) + " arguments but got " +
            //                      std::to_string(arguments.size()) + ".";
            // 抛出运行时错误，包含错误信息
            // throw runtime_error(callExpr->keyword, result);
            throw runtime_error(
                callExpr->keyword, ("Expected {} arguments but got {}." + std::to_string(callable->arity()) +
                                    std::to_string(arguments.size()))
            );
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
 * @brief 处理 BlockStmt 语句的调用运算符重载。
 *
 * 该函数执行代码块语句，创建一个新的环境来执行代码块中的语句。
 * 新的环境会继承当前环境的变量，但不会影响当前环境中的变量。
 *
 * @param blockStmt 指向 BlockStmt 的智能指针，包含要执行的语句列表。
 * @return StmtResult 执行结果，如果代码块中遇到 Return 语句则返回该结果，否则返回 Nothing。
 */
StmtResult Interpreter::operator()(const BlockStmtPtr &blockStmt) {
    return executeBlock(blockStmt->statements, std::make_shared<Environment>(environment));
}


/**
 * @brief 处理 ClassStmt 语句的调用运算符重载。
 *
 * 该函数执行类声明语句，处理类的继承关系，定义类的方法，并将类对象存储在环境中。
 *
 * @param classStmt 指向 ClassStmt 的智能指针，包含类定义信息。
 * @return StmtResult 执行结果，通常为 Nothing。
 */
StmtResult Interpreter::operator()(const ClassStmtPtr &classStmt) {
    // 处理父类
    std::optional<std::shared_ptr<LoxClass>> super_class;
    if (classStmt->super_class.has_value()) {
        if (const auto &s = (*this)(classStmt->super_class.value());
            std::holds_alternative<LoxCallablePtr>(s) && dynamic_cast<LoxClass *>(std::get<LoxCallablePtr>(s).get())) {
            super_class = std::reinterpret_pointer_cast<LoxClass>(std::get<LoxCallablePtr>(s));
        } else {
            throw runtime_error(classStmt->super_class.value()->name, "Superclass must be a class.");
        }
    }

    // 在环境中定义类名
    environment->define(classStmt->name.getLexeme());

    // 如果有父类，创建新的环境并定义super
    if (super_class.has_value()) {
        environment = std::make_shared<Environment>(environment);
        environment->define("super", super_class.value());
    }

    // 收集类的方法
    std::unordered_map<std::string_view, LoxFunctionPtr> methods;
    for (auto &method: classStmt->methods) {
        methods[method->name.getLexeme()] =
            std::make_shared<LoxFunction>(method, environment, method->type == LoxFunctionType::INITIALIZER);
    }

    // 如果有父类，恢复原来的环境
    if (super_class.has_value()) { environment = environment->get_enclosing(); }
    // 在环境中创建类对象
    environment->assign(
        classStmt->name, std::make_shared<LoxClass>(classStmt->name.getLexeme(), super_class, std::move(methods))
    );

    return Nothing();
}

/**
 * @brief 处理 GetExpr 表达式的调用运算符重载。
 *
 * 该函数用于获取对象实例的属性值。
 *
 * @param getExpr 指向 GetExpr 的智能指针，表示属性访问表达式。
 * @return LoxObject 获取到的属性值。
 */
LoxObject Interpreter::operator()(const GetExprPtr &getExpr) {
    if (const auto &object = evaluate(getExpr->object); std::holds_alternative<LoxInstancePtr>(object)) {
        return std::get<LoxInstancePtr>(object)->get(getExpr->name);
    }

    throw runtime_error(getExpr->name, "Only instances have properties.");
}

/**
 * @brief 处理 SetExpr 表达式的调用运算符重载。
 *
 * 该函数用于设置对象实例的属性值。
 *
 * @param setExpr 指向 SetExpr 的智能指针，表示属性赋值表达式。
 * @return LoxObject 设置后的属性值。
 */
LoxObject Interpreter::operator()(const SetExprPtr &setExpr) {
    const auto &object = evaluate(setExpr->object);

    if (!std::holds_alternative<LoxInstancePtr>(object)) {
        throw runtime_error(setExpr->name, "Only instances have fields.");
    }

    auto value = evaluate(setExpr->value);
    std::get<LoxInstancePtr>(object)->set(setExpr->name, value);
    return value;
}

/**
 * @brief 处理 SuperExpr 表达式的调用运算符重载。
 *
 * 该函数用于调用父类的方法。
 *
 * @param superExpr 指向 SuperExpr 的智能指针，表示父类方法调用表达式。
 * @return LoxObject 方法调用的返回值。
 */
LoxObject Interpreter::operator()(const SuperExprPtr &superExpr) const {
    // 获取父类对象
    const auto &callable = std::get<LoxCallablePtr>(environment->getAt(superExpr->distance, "super"));
    const auto &super_class = std::reinterpret_pointer_cast<LoxClass>(callable);
    // 获取当前实例
    const auto &instance = std::get<LoxInstancePtr>(environment->getAt(superExpr->distance - 1, "this"));
    // 查找父类方法
    const auto &method = super_class->findMethod(superExpr->method.getLexeme());
    if (method == nullptr) {
        throw runtime_error(superExpr->method, ("Undefined property" + std::string(superExpr->method.getLexeme())));
    }
    // 绑定实例并返回方法
    return method->bind(instance);
}


/**
 * @brief 处理 BinaryExpr 表达式的调用运算符重载。
 *
 * 该函数用于计算二元表达式的值，支持多种运算符操作。
 *
 * @param binaryExpr 指向 BinaryExpr 的智能指针，表示二元表达式。
 * @return LoxObject 二元表达式计算的结果。
 */
LoxObject Interpreter::operator()(const BinaryExprPtr &binaryExpr) {
    // 计算左右操作数的值
    const auto &left = evaluate(binaryExpr->left);
    const auto &right = evaluate(binaryExpr->right);

    // 根据运算符类型执行相应操作
    switch (binaryExpr->op) {
        case BinaryOp::PLUS: {
            // 处理加法运算
            if (std::holds_alternative<LoxNumber>(left) && std::holds_alternative<LoxNumber>(right)) {
                return std::get<LoxNumber>(left) + std::get<LoxNumber>(right);
            }

            // 处理字符串拼接
            if (std::holds_alternative<LoxString>(left) && std::holds_alternative<LoxString>(right)) {
                return std::get<LoxString>(left) + std::get<LoxString>(right);
            }

            // 如果操作数类型不匹配，抛出错误
            throw runtime_error(binaryExpr->token, "Operands must be two numbers or two strings.");
        }
        case BinaryOp::MINUS:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) - std::get<LoxNumber>(right);
        case BinaryOp::SLASH:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) / std::get<LoxNumber>(right);
        case BinaryOp::STAR:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) * std::get<LoxNumber>(right);
        case BinaryOp::GREATER:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) > std::get<LoxNumber>(right);
        case BinaryOp::GREATER_EQUAL:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) >= std::get<LoxNumber>(right);
        case BinaryOp::LESS:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) < std::get<LoxNumber>(right);
        case BinaryOp::LESS_EQUAL:
            checkNumberOperands(binaryExpr->token, left, right);
            return std::get<LoxNumber>(left) <= std::get<LoxNumber>(right);
        case BinaryOp::BANG_EQUAL:
            return left != right;
        case BinaryOp::EQUAL_EQUAL:
            return left == right;
    }

    // 如果遇到未处理的运算符，程序将不会执行到这里
    __builtin_unreachable();
}

/**
 * @brief 处理 ThisExpr 表达式的调用运算符重载。
 *
 * 该函数用于获取当前实例的引用。
 *
 * @param thisExpr 指向 ThisExpr 的智能指针，表示 this 表达式。
 * @return LoxObject 当前实例的引用。
 */
LoxObject Interpreter::operator()(const ThisExprPtr &thisExpr) const {
    return lookUpVariable(thisExpr->name, *thisExpr);
}

/**
 * @brief 处理 GroupingExpr 表达式的调用运算符重载。
 *
 * 该函数用于计算括号表达式中的值。
 *
 * @param groupingExpr 指向 GroupingExpr 的智能指针，表示括号表达式。
 * @return LoxObject 括号表达式计算的结果。
 */
LoxObject Interpreter::operator()(const GroupingExprPtr &groupingExpr) { return evaluate(groupingExpr->expression); }

/**
 * @brief 处理 LiteralExpr 表达式的调用运算符重载。
 *
 * 该函数用于处理字面量表达式，将字面量值转换为 LoxObject。
 *
 * @param literalExpr 指向 LiteralExpr 的智能指针，表示字面量表达式。
 * @return LoxObject 字面量值对应的 LoxObject。
 */
LoxObject Interpreter::operator()(const LiteralExprPtr &literalExpr) const {
    // 使用 std::visit 遍历 literalExpr->value 的变体类型
    return std::visit(
        // 定义一个 overloaded 结构体，用于处理不同类型的字面量
        overloaded{
            // 处理布尔类型字面量
            [](const bool value) -> LoxObject { return value; },
            // 处理双精度浮点类型字面量
            [](const double value) -> LoxObject { return value; },
            // 处理字符串视图类型字面量，将其转换为 std::string 类型
            [](const std::string_view value) -> LoxObject { return std::string(value); },
            // 处理空指针类型字面量，返回 LoxNil 类型
            [](const std::nullptr_t) -> LoxObject { return LoxNil(); },
        },
        literalExpr->value
    );
}

/**
 * @brief 查找变量的值。
 *
 * 该函数根据变量的作用域距离查找变量的值。如果距离为 -1，表示全局变量；
 * 否则，根据距离在环境中查找变量。
 *
 * @param name 变量的 Token。
 * @param expr 包含变量作用域距离的表达式。
 * @return LoxObject& 变量的引用。
 */
[[nodiscard]] LoxObject &Interpreter::lookUpVariable(const Token &name, const Assignable &expr) const {
    // 判断变量是否为全局变量
    if (expr.distance == -1) {
        // 如果是全局变量，从全局环境中获取变量的值
        return globals->get(name);
    }
    // 如果是局部变量，根据作用域距离从当前环境中获取变量的值
    return environment->getAt(expr.distance, name.getLexeme());
}

/**
 * @brief 处理 LogicalExpr 表达式的调用运算符重载。
 *
 * 该函数用于计算逻辑表达式的值，支持逻辑或（OR）和逻辑与（AND）操作。
 *
 * @param logicalExpr 指向 LogicalExpr 的智能指针，表示逻辑表达式。
 * @return LoxObject 逻辑表达式计算的结果。
 */
LoxObject Interpreter::operator()(const LogicalExprPtr &logicalExpr) {
    // 计算逻辑表达式的左操作数
    auto left = evaluate(logicalExpr->left);

    // 判断逻辑运算符类型
    if (logicalExpr->op == LogicalOp::OR) {
        // 如果是逻辑或运算符，且左操作数为真，则返回左操作数
        if (isTruthy(left)) { return left; }
    }
    if(logicalExpr->op == LogicalOp::AND){
        // 如果是逻辑与运算符，且左操作数为假，则返回左操作数
        if (!isTruthy(left)) { return left; }
    }

    // 如果左操作数不能确定逻辑表达式的结果，则计算右操作数并返回
    return evaluate(logicalExpr->right);
}

/**
 * @brief 处理 UnaryExpr 表达式的调用运算符重载。
 *
 * 该函数用于计算一元表达式的值，支持负号（MINUS）和逻辑非（BANG）操作。
 *
 * @param unaryExpr 指向 UnaryExpr 的智能指针，表示一元表达式。
 * @return LoxObject 一元表达式计算的结果。
 */
LoxObject Interpreter::operator()(const UnaryExprPtr &unaryExpr) {
    // 计算一元表达式的操作数
    const auto &result = evaluate(unaryExpr->expression);
    // 根据一元运算符类型进行相应操作
    switch (unaryExpr->op) {
        case UnaryOp::MINUS: {
            // 如果是负号运算符，检查操作数是否为数字类型，并返回其相反数
            return -checkNumberOperand(unaryExpr->token, result);
        }
        case UnaryOp::BANG:
            // 如果是逻辑非运算符，返回操作数的逻辑非结果
            return !isTruthy(result);
    }

    // 如果遇到未处理的运算符，程序将不会执行到这里
    __builtin_unreachable();
}

/**
 * @brief 处理 VarExpr 表达式的调用运算符重载。
 *
 * 该函数用于查找变量的值。
 *
 * @param varExpr 指向 VarExpr 的智能指针，表示变量表达式。
 * @return LoxObject 变量的值。
 */
LoxObject Interpreter::operator()(const VarExprPtr &varExpr) const {
    // 调用 lookUpVariable 函数查找变量的值
    return lookUpVariable(varExpr->name, *varExpr);
}


/**
 * @brief 处理 AssignExpr 表达式的调用运算符重载。
 *
 * 该函数用于给变量赋值，支持全局变量和局部变量的赋值。
 * 根据变量的作用域距离，决定是在全局环境还是局部环境中进行赋值操作。
 *
 * @param assignExpr 指向 AssignExpr 的智能指针，表示赋值表达式。
 * @return LoxObject 赋值后的值。
 */
LoxObject Interpreter::operator()(const AssignExprPtr &assignExpr) {
    // 计算赋值表达式右侧的值
    const auto &value = evaluate(assignExpr->value);
    // 判断变量是否为全局变量
    if (assignExpr->distance == -1) {
        // 如果是全局变量，在全局环境中进行赋值
        globals->assign(assignExpr->name, value);
    } else {
        // 如果是局部变量，根据作用域距离在局部环境中进行赋值
        environment->assignAt(assignExpr->distance, assignExpr->name, value);
    }
    // 返回赋值后的值
    return value;
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
StmtResult Interpreter::executeBlock(const StmtList &statements, const EnvironmentPtr &newenvironment) {
    // 保存原来的环境
    const auto previous = environment;
    // 设置新的环境
    environment = newenvironment;

    // 依次执行代码块中的每个语句
    for (const auto &statement: statements) {
        // 执行语句
        if (auto result = evaluate(statement); std::holds_alternative<Return>(result)) {
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

