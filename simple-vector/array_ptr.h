#pragma once
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size) {
        if (size == 0) {
            raw_ptr_ = nullptr;
        }
        else {
            raw_ptr_ = new Type[size];
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr) {
    }

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;

    ArrayPtr(ArrayPtr&& other) {
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& other) {
        if (this != &other) {
            raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
        }
        return *this;
    }

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept {
        if (index == 0) {
            return *raw_ptr_;
        }
        Type* adress = raw_ptr_ + index;
        return *adress;
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept {
        if (index == 0) {
            return *raw_ptr_;
        }
        Type* adress = raw_ptr_ + index;
        return *adress;
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const {
        if (raw_ptr_ != nullptr) {
            return true;
        }
        return false;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept {
        if (raw_ptr_) {
            return raw_ptr_;
        }
        return nullptr;
    }

    // Обменивается значениям указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept {
        std::swap(this->raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr;
};
