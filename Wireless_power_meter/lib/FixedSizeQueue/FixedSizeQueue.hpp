/*
 * @LastEditors: qingmeijiupiao
 * @Description: 定长队列
 * @Author: qingmeijiupiao
 * @LastEditTime: 2024-11-17 23:37:28
 */
#ifndef FIXEDSIZEQUEUE_HPP
#define FIXEDSIZEQUEUE_HPP

#include <deque>
#include <vector>
#include <stdexcept>
#include <queue>

template <typename T, size_t N>
class FixedSizeQueue {
private:
    std::deque<T> data;           // 主数据存储
    std::deque<T> max_deque;       // 维护最大值
    std::deque<T> min_deque;       // 维护最小值
    T sum = T();                   // 总和

public:
    FixedSizeQueue() = default;

    void push(const T& value) {
        // 队列已满时弹出最旧元素
        if (data.size() == N) {
            T front_value = data.front();
            
            // 更新最大值队列
            if (front_value == max_deque.front()) 
                max_deque.pop_front();
            else if (front_value > max_deque.front()) 
                max_deque.clear(); // 安全处理
            
            // 更新最小值队列
            if (front_value == min_deque.front()) 
                min_deque.pop_front();
            else if (front_value < min_deque.front()) 
                min_deque.clear(); // 安全处理
            
            sum -= front_value;
            data.pop_front();
        }

        // 添加新元素
        data.push_back(value);
        sum += value;

        // 更新最大值队列
        while (!max_deque.empty() && value > max_deque.back()) 
            max_deque.pop_back();
        max_deque.push_back(value);

        // 更新最小值队列
        while (!min_deque.empty() && value < min_deque.back()) 
            min_deque.pop_back();
        min_deque.push_back(value);
    }

    T back() const {
        if (data.empty()) throw std::out_of_range("Queue is empty");
        return data.back();
    }

    T front() const {
        if (data.empty()) throw std::out_of_range("Queue is empty");
        return data.front();
    }

    bool isEmpty() const {
        return data.empty();
    }

    size_t size() const {
        return data.size();
    }

    std::queue<T> copy() const {
        std::queue<T> q;
        for (const T& val : data) {
            q.push(val);
        }
        return q;
    }

    T get_average() const {
        if (data.empty()) throw std::out_of_range("Queue is empty");
        return sum / static_cast<T>(data.size());
    }

    T get_max() const {
        if (data.empty()) throw std::out_of_range("Queue is empty");
        return max_deque.front();
    }

    T get_min() const {
        if (data.empty()) throw std::out_of_range("Queue is empty");
        return min_deque.front();
    }

    std::vector<T> toVector() const {
        return std::vector<T>(data.begin(), data.end());
    }

    T* toArray() const {
        T* arr = new T[N](); // 初始化为0
        size_t i = 0;
        for (const T& val : data) {
            if (i >= N) break;
            arr[i++] = val;
        }
        return arr;
    }
};

#endif // FIXEDSIZEQUEUE_HPP
