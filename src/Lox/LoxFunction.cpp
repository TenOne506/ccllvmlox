#include "Lox/LoxFunction.h"

/**
 * @brief 重载函数调用运算符，用于执行 Lox 函数。
 * 
 * 该函数创建一个新的环境，将参数绑定到该环境中，然后执行函数体。
 * 如果函数是初始化器，它将返回 `this` 对象。
 * 
 * @param interpreter 解释器实例，用于执行函数体。
 * @param arguments 传递给函数的参数列表。
 * @return LoxObject 函数的返回值。
 */
LoxObject LoxFunction::operator()(Interpreter &interpreter, const llvm::SmallVector<LoxObject> &arguments) {
    // 创建一个新的环境，该环境的封闭环境为当前函数的闭包
    const auto environment = std::make_shared<Environment>(closure);
    // 遍历函数声明中的参数列表
    for (int i = 0; i < static_cast<int>(declaration->parameters.size()); i++) {
        // 将参数名和对应的参数值绑定到新环境中
        environment->define(declaration->parameters[i].getLexeme(), arguments[i]);
    }

    // 执行函数体，并获取执行结果
    if (const auto &result = interpreter.executeblock(declaration->body, environment);
        std::holds_alternative<Return>(result)) {
        // 如果函数是初始化器，返回 `this` 对象
        if (isInitializer) { return std::move(closure->getAt(0, "this")); }

        // 否则，返回函数的返回值
        return std::move(std::get<Return>(result).value);
    }

    // 如果函数是初始化器，返回 `this` 对象
    if (isInitializer) { return std::move(closure->getAt(0, "this")); }

    // 如果函数没有返回值，返回 LoxNil
    return LoxNil();
}

/**
 * @brief 将函数绑定到一个实例上，创建一个新的绑定函数。
 * 
 * 该函数创建一个新的环境，将 `this` 绑定到指定的实例上，然后返回一个新的 LoxFunction 实例。
 * 
 * @param instance 要绑定到函数的实例。
 * @return LoxFunctionPtr 绑定后的函数指针。
 */
LoxFunctionPtr LoxFunction::bind(const LoxInstancePtr &instance) {
    // 创建一个新的环境，该环境的封闭环境为当前函数的闭包
    auto environment = std::make_shared<Environment>(closure);
    // 将 `this` 绑定到指定的实例上
    environment->define("this", instance);
    // 返回一个新的 LoxFunction 实例，使用新的环境
    return std::make_shared<LoxFunction>(declaration, environment, isInitializer);
}

/**
 * @brief 将函数转换为字符串表示形式。
 * 
 * 该函数返回一个字符串，包含函数的名称。
 * 
 * @return std::string 函数的字符串表示形式。
 */
std::string LoxFunction::to_string() { 
    // 返回函数的字符串表示形式，格式为 "<fn {函数名}>"
    return "<fn {}>" + std::string(declaration->name.getLexeme()); 
}
