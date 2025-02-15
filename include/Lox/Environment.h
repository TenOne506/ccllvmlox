#pragma once

#include "Lox/LoxObject.h"
#include "frontend/Token.h"
#include <unordered_map>
/**
 * @brief 前置声明 Environment 类
 * 
 * 用于在定义智能指针类型时引用 Environment 类，避免头文件循环包含问题。
 */
class Environment;

/**
 * @brief 定义 Environment 类的智能指针类型
 * 
 * 使用 std::shared_ptr 来管理 Environment 对象的生命周期，方便内存管理和对象共享。
 */
using EnvironmentPtr = std::shared_ptr<Environment>;

/**
 * @brief 环境类，用于管理变量的定义和查找
 * 
 * 该类继承自 std::enable_shared_from_this，允许在类内部安全地获取指向自身的 std::shared_ptr。
 * 它维护一个变量名到值的映射，并支持嵌套环境，通过 enclosing 指针指向外部环境。
 */
class Environment : public std::enable_shared_from_this<Environment> {
private:
    /**
     * @brief 存储变量名到值的映射
     * 
     * 使用 std::unordered_map 来高效存储和查找变量名和对应的值。
     */
    std::unordered_map<std::string_view, LoxObject> values;

    /**
     * @brief 指向外部环境的智能指针
     * 
     * 用于实现嵌套环境，允许在当前环境中查找外部环境的变量。
     */
    EnvironmentPtr enclosing;
public:
    /**
     * @brief 默认构造函数
     * 
     * 创建一个没有外部环境的新环境。
     */
    explicit Environment();

    /**
     * @brief 带外部环境的构造函数
     * 
     * 创建一个新环境，并指定其外部环境。
     * @param environment 外部环境的智能指针
     */
    explicit Environment(EnvironmentPtr& environment);

    /**
     * @brief 获取外部环境
     * 
     * 返回当前环境的外部环境的智能指针。
     * @return 外部环境的智能指针
     */
    EnvironmentPtr get_enclosing() const { return enclosing; }

    /**
     * @brief 定义一个新变量
     * 
     * 在当前环境中定义一个新变量，并赋予指定的值。
     * @param name 变量名
     * @param value 变量的值，默认为 LoxNil 类型
     */
    void define(std::string_view name, const LoxObject &value = LoxNil{});

    /**
     * @brief 在指定距离的祖先环境中获取变量
     * 
     * 在距离当前环境指定距离的祖先环境中查找并返回指定名称的变量。
     * @param distance 距离当前环境的距离
     * @param name 变量名
     * @return 变量的引用
     */
    LoxObject &getAt(unsigned long distance, const std::string_view &name);

    /**
     * @brief 获取指定距离的祖先环境
     * 
     * 返回距离当前环境指定距离的祖先环境的智能指针。
     * @param distance 距离当前环境的距离
     * @return 祖先环境的智能指针
     */
    EnvironmentPtr ancestor(unsigned long distance);

    /**
     * @brief 获取变量的值
     * 
     * 在当前环境及其外部环境中查找并返回指定名称的变量。
     * @param name 变量的 Token 对象
     * @return 变量的引用
     */
    LoxObject &get(const Token &name);

    /**
     * @brief 为变量赋值
     * 
     * 在当前环境及其外部环境中查找指定名称的变量，并为其赋予新值。
     * @param name 变量的 Token 对象
     * @param value 新的值
     */
    void assign(const Token &name, const LoxObject &value);

    /**
     * @brief 在指定距离的祖先环境中为变量赋值
     * 
     * 在距离当前环境指定距离的祖先环境中查找指定名称的变量，并为其赋予新值。
     * @param distance 距离当前环境的距离
     * @param name 变量的 Token 对象
     * @param value 新的值
     */
    void assignAt(unsigned long distance, const Token &name, const LoxObject &value);


};