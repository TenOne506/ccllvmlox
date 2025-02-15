#pragma once
#include "Lox/Interpreter.h"
#include "Lox/LoxObject.h"
#include <llvm/ADT/SmallVector.h>

/**
 * @brief 表示 Lox 语言中可调用对象的抽象基类。
 * 
 * 该类定义了可调用对象的基本接口，包括函数调用、获取参数数量和转换为字符串表示的方法。
 */
class LoxCallable {
public:
    // 可调用对象的参数数量
    int _arity = 0;

    /**
     * @brief 构造函数，初始化可调用对象的参数数量。
     * 
     * @param arity 可调用对象的参数数量。
     */
    explicit LoxCallable(const int arity) : _arity{arity} {}

    /**
     * @brief 析构函数，声明为虚函数以确保正确的析构行为。
     */
    virtual ~LoxCallable() = default;

    /**
     * @brief 重载函数调用运算符，用于执行可调用对象。
     * 
     * 该方法必须在派生类中实现，以定义可调用对象的具体行为。
     * 
     * @param interpreter 解释器实例，用于执行可调用对象。
     * @param arguments 传递给可调用对象的参数列表。
     * @return LoxObject 可调用对象的返回值。
     */
    virtual LoxObject operator()(Interpreter &interpreter, const llvm::SmallVector<LoxObject> &arguments) = 0;

    /**
     * @brief 将可调用对象转换为字符串表示形式。
     * 
     * 该方法必须在派生类中实现，以提供可调用对象的字符串表示。
     * 
     * @return std::string 可调用对象的字符串表示。
     */
    virtual std::string to_string() = 0;

    /**
     * @brief 获取可调用对象的参数数量。
     * 
     * @return int 可调用对象的参数数量。
     */
    virtual int arity() { return this->_arity; };
};
