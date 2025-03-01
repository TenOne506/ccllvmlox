#include "Lox/LoxClass.h"
// 包含 LoxFunction 类的头文件
#include "Lox/LoxFunction.h"

/**
 * @brief 重载函数调用运算符，用于实例化类对象。
 * 
 * 该函数在类对象被调用时触发，创建一个类的实例，并调用其构造函数（如果存在）。
 * 
 * @param interpreter 解释器实例，用于执行代码。
 * @param arguments 传递给构造函数的参数列表。
 * @return LoxObject 返回创建的类实例。
 */
LoxObject LoxClass::operator()(Interpreter &interpreter, const std::vector<LoxObject> &arguments) {
    // 创建一个新的类实例，并将当前类的共享指针传递给它
    const auto &instance = std::make_shared<LoxInstance>(shared_from_this());
    // 检查类是否有构造函数
    if (const auto &initializer = this->initializer; initializer != nullptr) {
        // 将构造函数绑定到新创建的实例上
        const auto &function = initializer->bind(instance);
        // 将绑定后的函数转换为可调用对象
        const auto &callable = std::reinterpret_pointer_cast<LoxCallable>(function);
        // 调用构造函数，并传递解释器和参数
        (*callable)(interpreter, arguments);
    }
    // 返回创建的类实例
    return instance;
}

/**
 * @brief 查找类中指定名称的方法。
 * 
 * 该函数在类的方法表中查找指定名称的方法，如果未找到，则在父类中继续查找。
 * 
 * @param method_name 要查找的方法名称。
 * @return LoxFunctionPtr 如果找到方法，则返回方法的指针；否则返回 nullptr。
 */
LoxFunctionPtr LoxClass::findMethod(const std::string_view method_name) {
    // 检查当前类的方法表中是否包含指定名称的方法
    if (methods.contains(method_name)) { 
        // 如果包含，则返回该方法的指针
        return methods[method_name]; 
    }
    // 检查类是否有父类
    if (superClass.has_value()) { 
        // 如果有父类，则在父类中继续查找该方法
        return superClass.value()->findMethod(method_name); 
    }
    // 如果未找到方法，则返回 nullptr
    return nullptr;
}

/**
 * @brief 获取类的构造函数的参数数量。
 * 
 * 该函数返回类的构造函数所需的参数数量，如果类没有构造函数，则返回 0。
 * 
 * @return int 构造函数的参数数量。
 */
int LoxClass::arity() {
    // 检查类是否有构造函数
    return this->initializer == nullptr ? 0
        // 如果有构造函数，则返回其参数数量
        : std::reinterpret_pointer_cast<LoxCallable>(this->initializer)->arity();
}

/**
 * @brief 将类的名称转换为字符串。
 * 
 * 该函数返回类的名称的字符串表示。
 * 
 * @return std::string 类的名称。
 */
std::string LoxClass::to_string() { 
    // 返回类的名称的字符串表示
    return std::string(name); 
}
