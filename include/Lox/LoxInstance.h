#pragma once
#include "Lox/LoxClass.h"
#include "Lox/LoxObject.h"
#include <memory>
#include <unordered_map>
#include <utility>

/**
 * @brief 表示 Lox 语言中的类实例，继承自 std::enable_shared_from_this<LoxInstance>
 * 
 * 这个类封装了 Lox 类实例的核心功能，包括所属的类和实例的字段。
 */
class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
public:
    // 该实例所属的 Lox 类
    LoxClassPtr klass;
    // 存储实例的字段，键为字段名，值为字段的值
    std::unordered_map<std::string_view, LoxObject> fields;

    /**
     * @brief 构造一个新的 LoxInstance 对象
     * 
     * @param klass 该实例所属的 Lox 类
     */
    explicit LoxInstance(LoxClassPtr klass) : klass{std::move(klass)} {}

    /**
     * @brief 获取实例中指定名称的字段的值
     * 
     * @param name 字段的名称
     * @return LoxObject 字段的值
     */
    LoxObject get(const Token &name);

    /**
     * @brief 设置实例中指定名称的字段的值
     * 
     * @param name 字段的名称
     * @param value 要设置的值
     */
    void set(const Token &name, const LoxObject &value);

    /**
     * @brief 将实例转换为字符串表示
     * 
     * @return std::string 实例的字符串表示
     */
    [[nodiscard]] std::string to_string() const;
};
