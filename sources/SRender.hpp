#ifndef SRENDER_HPP
#define SRENDER_HPP
#include <Registry.hpp>

class SRender{
public:
    SRender();
    ~SRender();
    static void Init();
    static void Update(Registry &registry);
    static void Shutdown();
private:
    static void DrawStatistics();
};

#endif // SRENDER_HPP
