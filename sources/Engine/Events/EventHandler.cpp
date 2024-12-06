//
// Created by cam on 09/09/24.
//
#include "Engine/pch.hpp"
#include "EventHandler.hpp"
std::unique_ptr<EventHandler> EventHandler::s_Instance = nullptr;

EventHandler* EventHandler::GetInstance()
{
    if (s_Instance == nullptr)
    {
        s_Instance = std::make_unique<EventHandler>();
    }
    return s_Instance.get();
}

