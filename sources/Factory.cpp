//
// Created by cam on 19/07/24.
//

#include "Factory.hpp"
unsigned short Factory::CreatePlayer() {
    unsigned short player = m_Registry->createEntity();
    m_Registry->addComponent<CPlayer>(player, CPlayer());
}
