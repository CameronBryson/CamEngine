template <typename T>
bool Registry::hasSparseSet() const
{
    return mSparseSets.find(std::type_index(typeid(T))) != mSparseSets.end();
}

template <class T>
void Registry::createSparseSet()
{
    assert(mSparseSets.find(std::type_index(typeid(T))) == mSparseSets.end() && "Error: Sparse set already exists for this type.");
	mSparseSets.try_emplace(std::type_index(typeid(T)), std::make_unique<SparseSet<T>>());
    //mSparseSets[std::type_index(typeid(T))] = std::make_unique<SparseSet<T>>();
}

template <typename T>
SparseSet<T>& Registry::getSparseSet() const
{
    assert(mSparseSets.find(std::type_index(typeid(T))) != mSparseSets.end() && "Error: Sparse set does not exist for this type.");
    return *static_cast<SparseSet<T>*>(mSparseSets.at(std::type_index(typeid(T))).get());
}

template <typename T, typename... Args>
void Registry::addComponent(unsigned short id, Args&&... componentArgs)
{
    assert(mEntities.end() != std::find(mEntities.begin(), mEntities.end(), id) && "Entity does not exist.");
    auto& set = getSparseSet<T>();
    mCommandQueue.push(std::make_unique<AddComponentCommand<T, Args...>>(set, id, std::forward<Args>(componentArgs)...));
    //set.addItem(id, std::forward<Args>(componentArgs)...);
}

template <typename T>
T& Registry::getComponent(unsigned short id) const
{
    assert(hasComponent<T>(id) && "Entity does not have component.");
    return getSparseSet<T>().get_item(id);
}

template <typename... T>
std::vector<unsigned short> Registry::getEntityIDs() const
{
    std::vector<unsigned short> result;
    if( sizeof...(T) == 0 )
    {
	return result;
    }
    std::vector<ISparseSet*> sparse_sets = { &getSparseSet<T>()... };

    // Initialize result with the first component's IDs if available
    result = sparse_sets[0]->getIDs();

    // Find intersection across all component types
    for( size_t i = 1; i < sparse_sets.size(); ++i )
    {
	result = sparse_sets[i]->getIntersection(result);
    }

    return result;
}

template <typename T>
bool Registry::hasComponent(unsigned short id) const
{
    return getSparseSet<T>().hasItem(id);
}

template <typename T>
void Registry::removeComponent(unsigned short id)
{
    assert(mEntities.end() != std::find(mEntities.begin(), mEntities.end(), id) && "Entity does not exist.");
    assert(hasComponent<T>(id) && "Entity does not have component.");
    auto& set = getSparseSet<T>();
    mCommandQueue.push(std::make_unique<RemoveComponentCommand<T>>(set, id));
}