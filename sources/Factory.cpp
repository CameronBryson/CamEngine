//
// Created by cam on 19/07/24.
//

#include "Factory.hpp"
unsigned short Factory::CreatePlayer() {
    const unsigned short player = m_Registry->createEntity();
    m_Registry->addComponent<CPlayer>(player, CPlayer());
    return player;
}
