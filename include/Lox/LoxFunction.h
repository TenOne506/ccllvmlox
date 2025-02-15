#pragma once

#include <utility>

#include "Lox/LoxCallable.h"

/**
 * @brief 表示 Lox 语言中的函数对象。
 * 
 * 该类继承自 LoxCallable，实现了函数调用的相关功能。
 */
class LoxFunction final : public LoxCallable {
public:
    // 函数声明的智能指针
    std::shared_ptr<FunctionStmt> declaration;
    // 函数的闭包环境
    EnvironmentPtr closure;
    // 标记函数是否为初始化器
    bool isInitializer;

    /**
     * @brief 构造函数，初始化 LoxFunction 对象。
     * 
     * @param declaration 函数声明的智能指针。
     * @param closure 函数的闭包环境。
     * @param isInitializer 标记函数是否为初始化器，默认为 false。
     */
    explicit LoxFunction(
        const std::shared_ptr<FunctionStmt> &declaration, EnvironmentPtr closure, const bool isInitializer = false
    )
        : LoxCallable(static_cast<int>(declaration->parameters.size())), declaration{declaration},
          closure{std::move(closure)}, isInitializer{isInitializer} {}

    /**
     * @brief 析构函数，默认实现。
     */
    ~LoxFunction() override = default;

    /**
     * @brief 重载函数调用运算符，执行函数调用。
     * 
     * @param interpreter 解释器实例。
     * @param arguments 传递给函数的参数列表。
     * @return LoxObject 函数调用的返回值。
     */
    LoxObject operator()(Interpreter &interpreter, const llvm::SmallVector<LoxObject> &arguments) override;

    /**
     * @brief 将函数绑定到一个实例上。
     * 
     * @param instance 要绑定的实例。
     * @return LoxFunctionPtr 绑定后的函数指针。
     */
    LoxFunctionPtr bind(const LoxInstancePtr &instance);

    /**
     * @brief 将函数转换为字符串表示形式。
     * 
     * @return std::string 函数的字符串表示。
     */
    std::string to_string() override;
};
