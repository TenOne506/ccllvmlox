#include "Lox/LoxObject.h"
#include "Lox/LoxCallable.h"
#include "Lox/LoxInstance.h"
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <iomanip>

/**
 * @brief 判断一个 LoxObject 是否为真值。
 * 
 * 该函数根据 LoxObject 的类型来判断其是否为真值。
 * 在 Lox 语言中，nil 被视为假值，布尔值直接返回其本身的值，其他类型都被视为真值。
 * 
 * @param object 要判断的 LoxObject 对象。
 * @return bool 如果对象为真值则返回 true，否则返回 false。
 */
bool isTruthy(const LoxObject &object) {
    // 如果对象是 LoxNil 类型，则返回 false
    if (std::holds_alternative<LoxNil>(object)) { return false; }
    // 如果对象是 LoxBoolean 类型，则返回其布尔值
    if (std::holds_alternative<LoxBoolean>(object)) { return std::get<LoxBoolean>(object); }
    // 其他类型的对象都被视为真值
    return true;
}

/**
 * @brief 将 LoxObject 转换为字符串表示形式。
 * 
 * 该函数目前只是简单地返回 "TODO"，需要进一步实现具体的转换逻辑。
 * 
 * @param object 要转换的 LoxObject 对象。
 * @return std::string 对象的字符串表示形式。
 */
std::string to_string(const LoxObject &object) {
    // 目前只是占位符，需要实现具体的转换逻辑
    //return "TODO";
    return std::visit(
            overloaded{
                [](const LoxBoolean value) -> std::string { return value ? "true" : "false"; },
                [](const LoxNumber value) -> std::string { return ("{"+  std::to_string(value)+"}"); },
                [](const LoxString &value) -> std::string { return value; },
                [](const LoxCallablePtr &callable) -> std::string { return callable->to_string(); },
                [](const LoxInstancePtr &instance) -> std::string { return instance->to_string(); },
                [](LoxNil) -> std::string { return "nil"; },
            },
            object
        );
}
