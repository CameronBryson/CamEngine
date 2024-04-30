#ifndef MEMBERDELEGATE_HPP
#define MEMBERDELEGATE_HPP

template <typename Object, typename... Args>
class MemberDelegate{
typedef void (Object::*MemberFuncPtr)(Args...);
public:
  template<void (Object::*Func)(Args...)>
  void Bind(Object* objectInstance){
    m_function = Func;
    m_object = objectInstance;
  }
  void Execute(){
  }

private:
  Object* m_object;
  MemberFuncPtr m_function;
};

#endif // MEMBERDELEGATE_HPP
