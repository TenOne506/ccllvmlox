#include "Lox/LoxInstance.h"
#include "Lox/LoxObject.h"
#include <Lox/LoxFunction.h>
#include <string>
/**
 * @brief 获取实例的属性或方法
 * 
 * 该函数尝试获取实例的属性或方法。首先检查实例的字段中是否包含指定名称的属性，如果包含则返回该属性的值。
 * 如果字段中不包含该属性，则尝试在实例所属的类中查找同名的方法。如果找到方法，则将其绑定到当前实例并返回。
 * 如果既没有找到属性也没有找到方法，则抛出运行时错误。
 * 
 * @param name 要获取的属性或方法的名称的 Token
 * @return LoxObject 属性的值或绑定到当前实例的方法
 * @throws runtime_error 如果属性或方法未定义
 */
LoxObject LoxInstance::get(const Token &name) {
    // 检查实例的字段中是否包含指定名称的属性
    if (fields.contains(name.getLexeme())) { 
        // 如果包含，则返回该属性的值
        return fields[name.getLexeme()]; 
    }

    // 尝试在实例所属的类中查找同名的方法
    if (const auto method = klass->findMethod(name.getLexeme()); method != nullptr) {
        // 获取当前实例的共享指针
        const auto instance = shared_from_this();
        // 将方法绑定到当前实例，并将其转换为可调用对象返回
        return std::reinterpret_pointer_cast<LoxCallable>(method->bind(instance));
    }

    // 如果既没有找到属性也没有找到方法，则抛出运行时错误
    throw runtime_error(name, "Undefined property '" + std::string(name.getLexeme()) + "'.");
}

/**
 * @brief 设置实例的属性值
 * 
 * 该函数用于设置实例的指定属性的值。如果属性不存在，则会创建该属性。
 * 
 * @param name 要设置的属性的名称的 Token
 * @param value 要设置的属性的值
 */
void LoxInstance::set(const Token &name, const LoxObject &value) { 
    // 将指定名称的属性设置为指定的值
    fields[name.getLexeme()] = value; 
}

/**
 * @brief 将实例转换为字符串表示
 * 
 * 该函数返回实例的字符串表示，格式为 "类名 instance"。
 * 
 * @return std::string 实例的字符串表示
 */
std::string LoxInstance::to_string() const { 
    // 返回实例的字符串表示，格式为 "类名 instance"
    return (std::string(this->klass->name) +"instance"); 
}
