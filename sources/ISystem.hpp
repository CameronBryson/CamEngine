#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

class ISystem {
public:
    virtual ~ISystem() = default;

private:
    virtual void Update()=0;
  virtual void Shutdown()=0;
};

#endif // ISYSTEM_HPP
