#pragma once
#include "Lox/LoxCallable.h"
#include "Lox/LoxInstance.h"
#include <memory>
/**
 * @brief 表示 Lox 语言中的类，继承自 LoxCallable 和 std::enable_shared_from_this<LoxClass>
 * 
 * 这个类封装了 Lox 类的核心功能，包括类名、父类、方法和构造函数。
 */
class LoxClass final :public LoxCallable,public std::enable_shared_from_this<LoxClass> {
public:
    // 类的名称
    std::string_view name;
    // 可选的父类，如果没有父类则为 std::nullopt
    std::optional<std::shared_ptr<LoxClass>> superClass;
    // 存储类的方法，键为方法名，值为方法的智能指针
    std::unordered_map<std::string_view, LoxFunctionPtr> methods;
    // 类的构造函数
    LoxFunctionPtr initializer;

    /**
     * @brief 构造一个新的 LoxClass 对象
     * 
     * @param name 类的名称
     * @param superClass 可选的父类
     * @param methods 类的方法列表
     */
    explicit LoxClass(
        const std::string_view &name, const std::optional<std::shared_ptr<LoxClass>> &superClass,
        const std::unordered_map<std::string_view, LoxFunctionPtr> &methods
    )
        : LoxCallable(0), name{name}, superClass{superClass}, methods{methods} {
        // 查找并设置类的构造函数
        this->initializer = findMethod("init");
    }

    /**
     * @brief 析构函数，默认实现
     */
    ~LoxClass() override = default;

    /**
     * @brief 调用类的实例化方法
     * 
     * @param interpreter 解释器实例
     * @param arguments 构造函数的参数列表
     * @return LoxObject 类的实例对象
     */
    LoxObject operator()(Interpreter &interpreter, const std::vector<LoxObject> &arguments) override;

    /**
     * @brief 查找类的方法
     * 
     * @param method_name 方法名
     * @return LoxFunctionPtr 方法的智能指针，如果未找到则为 nullptr
     */
    LoxFunctionPtr findMethod(std::string_view method_name);

    /**
     * @brief 获取类的构造函数的参数数量
     * 
     * @return int 构造函数的参数数量
     */
    int arity() override;

    /**
     * @brief 将类的名称转换为字符串
     * 
     * @return std::string 类的名称
     */
    std::string to_string() override;
};
