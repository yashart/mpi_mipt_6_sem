#include "thread"
#include "iostream"
#include <initializer_list>
#include <utility>
#include <future>
#include <experimental/any>

class simpleAsync {
private:
  std::experimental::any returnValue;
  std::thread asyncThread;
public:
  template<typename Function, typename... T>
  simpleAsync( Function& func, const T&... args) {
    asyncThread = std::thread([&] {returnValue = std::experimental::any(func(args...));});
  }
  std::experimental::any get() {
    asyncThread.join();
    return returnValue;
  }
};

std::string hello (int a) {
  std::cout << "hello, " << a << std::endl;
  return "hello, any!";
}

int main() {
  simpleAsync my_async(hello, 10);
  std::cout << std::experimental::any_cast<std::string&>( my_async.get());
}
