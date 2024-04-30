#ifndef DELEGATEDATAFUNC_HPP
#define DELEGATEDATAFUNC_HPP

#include "IDelegateData.hpp"
template<class ReturnType, class... Args>
class DelegateData<ReturnType, ReturnType(Args...)> : public IDelegateData{
public:
  typedef ReturnType(*FunctionPtr)(Args...);
  DelegateData(FunctionPtr pFunction): m_pFunction(pFunction) {}
private:
  FunctionPtr m_pFunction;
};

#endif // DELEGATEDATAFUNC_HPP
