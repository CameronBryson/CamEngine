#pragma once
#include "Event.hpp"
enum class InputEvents
{
    KeyPress,
    KeyRelease,
    ResetKeyStates
    //KeyStart,
    //KeyHold,
    //KeyEnd
};
class KeyPressEvent final : public Event<InputEvents>
{
public:
    explicit KeyPressEvent(int key) : Event<InputEvents>(InputEvents::KeyPress, "KeyPress"), key(key) {};
    int key;
};
class KeyReleaseEvent final : public Event<InputEvents>
{
public:
    explicit KeyReleaseEvent(int key) : Event<InputEvents>(InputEvents::KeyRelease, "KeyRelease"), key(key) {};
    int key;
};
class ResetKeyStatesEvent final : public Event<InputEvents>
{
public:
    explicit ResetKeyStatesEvent() : Event<InputEvents>(InputEvents::ResetKeyStates, "ResetKeyStates") {};
};
// class KeyStartEvent final : public Event<InputEvents>
// {
// public:
//     explicit KeyStartEvent(int key) : Event<InputEvents>(InputEvents::KeyStart, "KeyStart"), key(key) {};
//     int key;
// };
// class KeyHoldEvent final : public Event<InputEvents>
// {
// public:
//     explicit KeyHoldEvent(int key) : Event<InputEvents>(InputEvents::KeyHold, "KeyHold"), key(key) {};
//     int key;
// };
// class KeyEndEvent final : public Event<InputEvents>
// {
// public:
//     explicit KeyEndEvent(int key) : Event<InputEvents>(InputEvents::KeyEnd, "KeyEnd"), key(key) {};
//     int key;
// };
