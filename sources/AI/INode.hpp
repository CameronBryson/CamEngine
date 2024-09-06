#pragma once
struct status{
    enum{
        running,
        success,
        failure
    } status;
    float duration;
};
class i_node{
public:
    virtual void start() = 0;
    virtual status update(float dt) = 0;
    virtual void terminate(status status) = 0;
};