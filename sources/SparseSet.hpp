//
// Created by cam on 07/05/24.
//

#ifndef SPARSESET_HPP
#define SPARSESET_HPP
#include <vector>
#include <string>
#include <typeinfo>



#include "GameSettings.hpp"
#include "ISparseSet.hpp"
template<class T>
class SparseSet : public ISparseSet {
public:
    SparseSet() :
    m_Sparse(std::vector<unsigned short>(Settings::MAX_ENTITIES,0)),
    m_Items(std::vector<T>(Settings::MAX_COMPONENTS)){
        printf("Sparse set created of type: ");
        printf(typeid(T).name());
        printf("\n");

    }
    ~SparseSet() override {
        printf("Sparse set destroyed of type: ");
        printf(typeid(T).name());
        printf("\n");
    }
    void addItem(unsigned short ID, T item);
    void removeItem(unsigned short ID) override;
    bool hasItem(unsigned short ID) override;
    T &getItem(unsigned short ID);
    std::vector<unsigned short> getIDS();
    std::vector<T> &getItems();
private:
    std::vector<unsigned short> m_Sparse;
    std::vector<unsigned short> m_Dense;
    std::vector<T> m_Items;
};
template<class T>
void SparseSet<T>::addItem(unsigned short ID, T item) {
    const auto pos = m_Dense.size();
    m_Dense.push_back(ID);
    m_Items[ID] = item;
    //m_Items.push_back(item);
    m_Sparse[ID] = pos;

}
template<class T>
void SparseSet<T>::removeItem(unsigned short ID) {
    const auto last = m_Dense.back();
    std::swap(m_Dense.back(), m_Dense[m_Sparse[ID]]);
    std::swap(m_Items.back(), m_Items[m_Sparse[ID]]);
    std::swap(m_Sparse[last], m_Sparse[ID]);
    m_Dense.pop_back();
    //m_Items.pop_back();
}
template<class T>
bool SparseSet<T>::hasItem(unsigned short ID) {
    return ID < m_Sparse.size() && m_Sparse[ID] < m_Dense.size() && m_Dense[m_Sparse[ID]] == ID;
}
template<class T>
T &SparseSet<T>::getItem(unsigned short ID) {
    return m_Items[m_Sparse[ID]];
}
template<class T>
std::vector<unsigned short> SparseSet<T>::getIDS() {
    return m_Dense;
}
template<class T>
std::vector<T> &SparseSet<T>::getItems() {
    //this needs to change if i have dead components
    return m_Items;
}
#endif //SPARSESET_HPP
