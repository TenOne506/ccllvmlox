// 防止头文件被重复包含
#ifndef UTIL_H
#define UTIL_H

/**
 * @brief 定义一个模板结构体 overloaded，用于实现对多个可调用对象的重载。
 * 
 * 该结构体继承自多个可调用对象类型 Ts，并使用折叠表达式展开它们的 operator()，
 * 从而允许一个对象可以根据传入的参数类型调用不同的可调用对象。
 * 
 * @tparam Ts 可变参数模板，表示多个可调用对象的类型。
 */
template<class... Ts>
struct overloaded : Ts... {
    // 使用折叠表达式展开所有基类的 operator()
    using Ts::operator()...;
};

/**
 * @brief 模板推导指引，用于自动推导 overloaded 结构体的模板参数。
 * 
 * 当使用 overloaded 结构体时，可以直接传入多个可调用对象，编译器会自动推导模板参数。
 * 
 * @tparam Ts 可变参数模板，表示多个可调用对象的类型。
 */
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

/**
 * @brief 定义一个不可复制的基类 Uncopyable。
 * 
 * 该类通过删除拷贝构造函数和拷贝赋值运算符，防止派生类对象被复制。
 * 派生类可以继承该类来实现不可复制的特性。
 */
class Uncopyable {
public:
    // 删除拷贝构造函数，防止对象被复制构造
    Uncopyable(const Uncopyable &) = delete;
    // 删除拷贝赋值运算符，防止对象被赋值复制
    Uncopyable &operator=(const Uncopyable &) = delete;

protected:
    // 默认构造函数，允许派生类使用默认构造
    Uncopyable() = default;
    // 默认析构函数，允许派生类使用默认析构
    ~Uncopyable() = default;
};

/**
 * @brief 定义一个模板函数 to，用于将一个范围转换为指定的容器类型。
 * 
 * 该函数接收一个范围对象，并使用该范围的开始和结束迭代器构造一个指定类型的容器。
 * 
 * @tparam ContainerT 目标容器的类型。
 * @tparam RangeT 范围对象的类型。
 * @param range 要转换的范围对象。
 * @return ContainerT 转换后的容器对象。
 */
template<typename ContainerT, typename RangeT>
ContainerT to(RangeT &&range) {
    // 使用范围的开始和结束迭代器构造目标容器
    return ContainerT(begin(range), end(range));
}

// 结束头文件保护
#endif//UTIL_H