#include "Lox/Environment.h"
#include "Lox/LoxObject.h"
#include <llvm/Support/raw_ostream.h>


/**
 * @brief 默认构造函数，创建一个没有封闭环境的新环境。
 * 
 * 此构造函数初始化一个新的环境对象，其封闭环境指针设置为 nullptr，表示这是一个顶级环境。
 */
Environment::Environment() : enclosing{nullptr} {}

/**
 * @brief 带参数的构造函数，创建一个具有指定封闭环境的新环境。
 * 
 * 此构造函数初始化一个新的环境对象，并将其封闭环境指针设置为传入的环境指针。
 * 这允许创建嵌套的环境，其中新环境可以访问其封闭环境中的变量。
 * 
 * @param environment 指向封闭环境的指针。
 */
Environment::Environment(EnvironmentPtr &environment) : enclosing{environment} {}

/**
 * @brief 获取指定名称的变量值
 * 
 * 该函数尝试从当前环境中查找指定名称的变量。如果找到，则返回该变量的值；
 * 如果未找到且存在封闭环境，则递归调用封闭环境的get方法继续查找；
 * 如果最终仍未找到，则抛出运行时错误。
 * 
 * @param name 要查找的变量的Token对象
 * @return LoxObject& 找到的变量的值的引用
 * @throws runtime_error 如果变量未定义
 */
LoxObject &Environment::get(const Token &name) {
    // 检查当前环境中是否存在该变量
    if (values.find(name.getLexeme()) != values.end()) {
        // 如果存在，返回该变量的值
        return values[name.getLexeme()];
    }

    // 检查是否存在封闭环境
    if (enclosing != nullptr) {
        // 如果存在，递归调用封闭环境的get方法继续查找
        return enclosing->get(name);
    }
    // 如果未找到变量，输出错误信息
    llvm::errs() << name.toString() << "Undefined variable '" << std::string(name.getLexeme()) << "'.";
    // 抛出运行时错误
    throw runtime_error(name, "Undefined variable '" + std::string(name.getLexeme()) + "'.");
}

/**
 * @brief 在当前环境中定义一个新的变量
 * 
 * 该函数将指定名称和值的变量添加到当前环境中。
 * 
 * @param name 要定义的变量的名称
 * @param value 要定义的变量的值
 */
void Environment::define(std::string_view name, const LoxObject &value) {
    // 将变量添加到当前环境中
    values[name] = value;
}

/**
 * @brief 在当前环境或其封闭环境中为变量赋值
 * 
 * 该函数尝试在当前环境中查找指定名称的变量。如果找到，则更新该变量的值；
 * 如果未找到且存在封闭环境，则递归调用封闭环境的assign方法继续查找并赋值；
 * 如果最终仍未找到，则抛出运行时错误。
 * 
 * @param name 要赋值的变量的Token对象
 * @param value 要赋给变量的值
 * @throws runtime_error 如果变量未定义
 */
void Environment::assign(const Token &name, const LoxObject &value) {
    // 检查当前环境中是否存在该变量
    if (values.find(name.getLexeme()) != values.end()) {
        // 如果存在，更新该变量的值
        values[name.getLexeme()] = value;
        return;
    }
    // 检查是否存在封闭环境
    if (enclosing != nullptr) {
        // 如果存在，递归调用封闭环境的assign方法继续查找并赋值
        enclosing->assign(name, value);
        return;
    }
    // 如果未找到变量，输出错误信息
    llvm::errs() << name.toString() << "Undefined variable '" << std::string(name.getLexeme()) << "'.";
    // 抛出运行时错误
    throw runtime_error(name, "Undefined variable '" + std::string(name.getLexeme()) + "'.");
}


LoxObject &Environment::getAt(const unsigned long distance, const std::string_view &name) {
    return ancestor(distance)->values[name];
}

EnvironmentPtr Environment::ancestor(const unsigned long distance) {
    auto environment = shared_from_this();
    for (unsigned long i = 0; i < distance; i++) { environment = environment->enclosing; }
    return environment;
}


void Environment::assignAt(const unsigned long distance, const Token &name, const LoxObject &value) {
    ancestor(distance)->values[name.getLexeme()] = value;
}