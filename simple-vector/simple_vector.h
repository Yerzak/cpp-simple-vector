#pragma once

#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

class ReserveProxyObj {
public: ReserveProxyObj(size_t capacity_to_reserve) : capacity_to_reserve_(capacity_to_reserve) {
}
      size_t capacity_to_reserve_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    ReserveProxyObj result(capacity_to_reserve);
    return result;
}

template <typename Type>
class SimpleVector {

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : arr(size), capacity(size), size_(size) {
        std::fill(begin(), end(), Type());
    }

    explicit SimpleVector(ReserveProxyObj object) {
        Reserve(object.capacity_to_reserve_);
    }

    void Reserve(size_t new_capacity) {
        size_t temp = size_;
        if (new_capacity > capacity) {
            SimpleVector <Type> new_vec(new_capacity);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), new_vec.begin());
            swap(new_vec);
            size_ = temp;
        }
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value)
        : arr(size), capacity(size), size_(size) {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : arr(init.size()), capacity(init.size()), size_(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) {
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && arr.Get() == nullptr);
        SimpleVector<Type> tmp(other.size_);
        //скопировать внутрь tmp элементы other
        std::copy(other.begin(), other.end(), tmp.begin());
        // После того как элементы скопированы, обмениваем данные текущего списка и tmp
        swap(tmp);
    }

    SimpleVector(SimpleVector&& other) {
        // Сначала надо удостовериться, что текущий список пуст
        assert(size_ == 0 && arr.Get() == nullptr);
        arr = std::move(other.arr);
        size_ = std::exchange(other.size_, 0);
        capacity = std::exchange(other.capacity, 0);
    }

    SimpleVector& operator=(const SimpleVector& other) {
        // Напишите тело конструктора самостоятельно
        if (this != &other) {
            auto tmp(other);
            swap(tmp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& other) {
        // Напишите тело конструктора самостоятельно
        if (this != &other) {
            arr = std::move(other.arr);
            size_ = std::exchange(other.size_, 0);
            capacity = std::exchange(other.capacity, 0);
        }
        return *this;
    }

    void swap(SimpleVector& other) noexcept {
        arr.swap(other.arr);
        std::swap(size_, other.size_);
        std::swap(capacity, other.capacity);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        // Напишите тело самостоятельно
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        // Напишите тело самостоятельно
        return capacity;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return arr[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        return (index >= size_ ? throw std::out_of_range(std::string("invalid index")) : arr[index]);
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        return (index >= size_ ? throw std::out_of_range(std::string("invalid index")) : arr[index]);
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size < capacity) {
            size_ = new_size;
        }
        else {
            size_t new_capacity = std::max(new_size, capacity * 2);
            ArrayPtr<Type> tmp(new_capacity);
            std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), tmp.Get());
            for (size_t i = size_; i < new_size; ++i) {
                tmp[i] = std::move(Type());
            }
            arr.swap(tmp);
            capacity = new_capacity;
            size_ = new_size;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return arr.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return (arr.Get() + size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return arr.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return (arr.Get() + size_);
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return arr.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return (arr.Get() + size_);
    }

    void PushBack(const Type& item) {
        if (capacity <= size_) {
            capacity = capacity * 2;
        }
        Resize(size_ + 1);
        arr[size_ - 1] = item;
    }

    void PushBack(Type&& item) {
        if (capacity <= size_) {
            capacity = capacity * 2;
        }
        Resize(size_ + 1);
        arr[size_ - 1] = std::move(item);
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        assert(pos >= begin() && pos < end());
        if (capacity == 0) {
            PushBack(value);
            return begin();
        }
        else {
            size_t index = pos - begin();
            if (size_ == capacity) {
                size_t tmp = size_;
                Resize(capacity * 2);
                size_ = tmp;
            }
            if (index == size_) {
                PushBack(value);
            }
            else {
                auto temp(this);
                size_++;
                std::copy_backward(temp->cbegin() + index, temp->cend() - 1, end());
                arr[index] = value;
            }
            return begin() + index;
        }
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        assert(pos >= begin() && pos < end());
        if (capacity == 0) {
            PushBack(std::move(value));
            return begin();
        }
        size_t index = pos - begin();
        if (index == size_) {
            PushBack(std::move(value));
            return begin() + index;
        }
        if (index > size_) {
            throw std::out_of_range(std::string("cannot seek vector iterator after end"));
        }
        if (size_ == capacity) {
            size_t tmp = size_;
            Resize(capacity * 2);
            size_ = tmp;
        }
        for (size_t i = size_; i > index; --i) {
            arr[i] = std::move(arr[i - 1]);
        }
        arr[index] = std::move(value);
        ++size_;
        return begin() + index;
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(pos >= begin() && pos < end());
        size_t position = pos - begin();
        assert(position < size_);
        std::copy(std::make_move_iterator(begin() + position + 1), std::make_move_iterator(end()), begin() + position);
        --size_;
        return begin() + position;
    }

private:
    ArrayPtr <Type> arr;
    size_t capacity = 0;
    size_t size_ = 0;


};

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if (&lhs == &rhs) {
        return true;
    }
    else if (lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (rhs < lhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}