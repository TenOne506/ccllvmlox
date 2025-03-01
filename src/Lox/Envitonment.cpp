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
Environment::Environment(EnvironmentPtr environment) : enclosing{std::move(environment)}  {}

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
    if (values.contains(name.getLexeme())) {
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
    if (values.contains(name.getLexeme())) {
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


/**
 * @brief 根据指定的距离获取特定环境中变量的值
 * 
 * 该函数通过调用 `ancestor` 方法找到指定距离处的环境，然后从该环境中获取指定名称的变量的值。
 * 
 * @param distance 距离当前环境的层数，用于定位目标环境
 * @param name 要查找的变量的名称
 * @return LoxObject& 返回找到的变量的值的引用
 */
LoxObject &Environment::getAt(const unsigned long distance, const std::string_view &name) {
    // 调用 ancestor 方法找到指定距离处的环境，并获取该环境中指定名称的变量的值
    return ancestor(distance)->values[name];
}

/**
 * @brief 根据指定的距离找到祖先环境
 * 
 * 该函数从当前环境开始，通过 `enclosing` 指针逐层向上查找，直到达到指定的距离，返回找到的祖先环境的指针。
 * 
 * @param distance 距离当前环境的层数，用于定位目标环境
 * @return EnvironmentPtr 返回找到的祖先环境的指针
 */
EnvironmentPtr Environment::ancestor(const unsigned long distance) {
    // 获取当前环境的共享指针
    auto environment = shared_from_this();
    // 循环指定的次数，通过 enclosing 指针逐层向上查找
    for (unsigned long i = 0; i < distance; i++) { 
        environment = environment->enclosing; 
    }
    // 返回找到的祖先环境的指针
    return environment;
}

/**
 * @brief 根据指定的距离在特定环境中为变量赋值
 * 
 * 该函数通过调用 `ancestor` 方法找到指定距离处的环境，然后在该环境中为指定名称的变量赋新值。
 * 
 * @param distance 距离当前环境的层数，用于定位目标环境
 * @param name 要赋值的变量的 Token 对象
 * @param value 要赋给变量的新值
 */
void Environment::assignAt(const unsigned long distance, const Token &name, const LoxObject &value) {
    // 调用 ancestor 方法找到指定距离处的环境，并在该环境中为指定名称的变量赋新值
    ancestor(distance)->values[name.getLexeme()] = value;
}
