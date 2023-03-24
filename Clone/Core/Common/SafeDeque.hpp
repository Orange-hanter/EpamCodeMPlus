#pragma once

#include <deque>
#include <mutex>

namespace Clone {

template <class T, class Allocator = std::allocator<T>>
class ThreadSafeDeque {
 public:
  void pop_front_waiting(T &t)
  {
    // unique_lock can be unlocked, lock_guard can not
    std::unique_lock<std::mutex> lock{mutex_};  // locks
    while (deque_.empty()) {
      cv_.wait(lock);  // unlocks, sleeps and relocks when woken up
    }
    t = deque_.front();
    deque_.pop_front();
  }  // unlocks as goes out of scope

  void push_back(T &&t)
  {
    std::unique_lock<std::mutex> lock{mutex_};
    deque_.push_back(std::move(t));
    lock.unlock();
    cv_.notify_one();  // wakes up pop_front_waiting
  }

  bool empty()
  {
    std::unique_lock<std::mutex> lock{mutex_};
    return deque_.empty();
  }

  T front()
  {
    std::unique_lock<std::mutex> lock{mutex_};
    return deque_.front();
  }

 private:
  std::deque<T, Allocator> deque_ = {};
  std::mutex mutex_ = {};
  std::condition_variable cv_ = {};
};
}  // namespace Clone