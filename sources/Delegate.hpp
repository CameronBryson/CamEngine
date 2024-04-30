#ifndef DELEGATE_HPP
#define DELEGATE_HPP

#include "IDelegateData.hpp"
template<typename... Args>
class Delegate{
typedef void (*FuncPtr)(Args...);
public:
    template<void (*Func)(Args...)>
    void Bind(){
      m_function = Func;
    }
    void (*Execute)(){

    };
    void Test(){
      Execute();
    }
private:
  FuncPtr m_function;
};

#endif // DELEGATE_HPP
