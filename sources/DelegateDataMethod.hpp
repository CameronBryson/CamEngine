#ifndef DELEGATEDATAMETHOD_HPP
#define DELEGATEDATAMETHOD_HPP

#include "IDelegateData.hpp"
template<class ReturnType, class ObjectType, class... Args>
class DelegateData<ReturnType, ObjectType, ReturnType(Args...)>:public IDelegateData{
public:
  typedef ReturnType(ObjectType::*MethodPtr)(Args...);
  DelegateData(ObjectType* pObj, MethodPtr pMethod): m_pObj(pObj), m_pMethod(pMethod) {}
private:
  ObjectType* m_pObj;
  MethodPtr m_pMethod;
};

#endif // DELEGATEDATAMETHOD_HPP
