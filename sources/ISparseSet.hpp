//
// Created by cam on 07/05/24.
//

#ifndef ISPARSESET_HPP
#define ISPARSESET_HPP
class ISparseSet {
public:
    virtual void removeItem(unsigned short ID) = 0;
    virtual bool hasItem(unsigned short ID) = 0;
};
#endif //ISPARSESET_HPP
