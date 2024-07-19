//
// Created by cam on 19/07/24.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP
#include <Registry.hpp>
#include <Components.hpp>


class Factory {
public:
    Factory(Registry* registry) : m_Registry(registry) {}
    ~Factory() {}

    unsigned short CreatePlayer();
private:
    Registry* m_Registry;
};



#endif //FACTORY_HPP
