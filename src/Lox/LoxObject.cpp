#include "Lox/LoxObject.h"

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
    return "TODO";
}
