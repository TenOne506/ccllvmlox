#pragma once
#include "Lox/LoxCallable.h"
#include <functional>
#include <llvm/ADT/SmallVector.h>
/**
 * @brief 表示 Lox 语言中的原生函数类。
 * 
 * 该类继承自 LoxCallable，用于封装原生 C++ 函数，使其可以在 Lox 解释器中被调用。
 */
class NativeFunction final : public LoxCallable {
public:
    // 定义原生函数的类型，使用 std::function 封装，接受一个 llvm::SmallVector<LoxObject> 类型的参数列表，并返回一个 LoxObject。
    using NativeFnType = std::function<LoxObject(const llvm::SmallVector<LoxObject> &)>;
    // 存储原生函数的实例。
    NativeFnType function;

    /**
     * @brief 构造函数，初始化原生函数对象。
     * 
     * @param function 要封装的原生函数。
     * @param arity 函数的参数数量，默认为 0。
     */
    explicit NativeFunction(NativeFnType function, const int arity = 0)
        : LoxCallable(arity), function{std::move(function)} {}

    /**
     * @brief 析构函数，默认实现。
     */
    ~NativeFunction() override = default;

    /**
     * @brief 重载函数调用运算符，执行原生函数。
     * 
     * @param interpreter 解释器实例，此处未使用。
     * @param arguments 传递给原生函数的参数列表。
     * @return LoxObject 原生函数的返回值。
     */
    LoxObject operator()(Interpreter & /*interpreter*/, const llvm::SmallVector<LoxObject> &arguments) override {
        return function(arguments);
    }

    /**
     * @brief 将原生函数转换为字符串表示形式。
     * 
     * @return std::string 原生函数的字符串表示，固定为 "<native fn>"。
     */
    std::string to_string() override { return "<native fn>"; }
};
