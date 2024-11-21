template <class T>
SparseSet<T>::SparseSet()
{
	m_Sparse.resize(settings::max_entities);
	//m_Dense.resize(MAX_ENTITIES);
	//m_Items.resize(MAX_ENTITIES);
	m_Dense.reserve(settings::max_entities);
	m_Items.reserve(settings::max_entities);
}

template <class T>
template <typename... Args>
void SparseSet<T>::addItem(unsigned short entityID, Args&&... componentArgs)
{

	if( entityID >= settings::max_entities )
	{
	throw std::out_of_range("Exceeded maximum capacity of SparseSet.");
	}
	m_Dense.emplace_back(entityID);
	m_Items.emplace_back(std::forward<Args>(componentArgs)...);
	//m_Items.emplace_back(std::forward<Args>(componentArgs)...);
	m_Sparse[entityID] = m_Dense.size() - 1;
	EventHandler::GetInstance()->GetComponentDispatcher<T>().SendEvent(ComponentAddedEvent(entityID));
}


template <class T>
void SparseSet<T>::removeItem(const unsigned short entityID)
{
	unsigned entityIndex = m_Sparse[entityID];
	unsigned lastIndex = m_Dense.size() - 1;
	unsigned lastEntity = m_Dense[lastIndex];

	std::swap(m_Dense[entityIndex], m_Dense[lastIndex]);
	std::swap(m_Items[entityIndex], m_Items[lastIndex]);

	// Remove the last item
	m_Dense.pop_back();
	m_Items.pop_back();

	m_Sparse[lastEntity] = entityIndex;
	m_Sparse[entityID] = settings::max_entities; // Mark as removed or invalid
	EventHandler::GetInstance()->GetComponentDispatcher<T>().SendEvent(ComponentRemovedEvent(entityID));
}

template <class T>
bool SparseSet<T>::hasItem(const unsigned short entityID)
{
	return m_Sparse[entityID] < m_Dense.size() && m_Dense[m_Sparse[entityID]] == entityID;
}

template <class T>
T& SparseSet<T>::get_item(const unsigned short entityID)
{
	auto index = m_Sparse[entityID];
	if( index >= getSize() )
	{
	throw std::out_of_range("Entity not found in SparseSet.");
	}
	return m_Items[index];
}

template <class T>
int SparseSet<T>::getSize() const
{
	return m_Dense.size();
}

template <class T>
std::vector<unsigned short> SparseSet<T>::getIDs() const
{
	return m_Dense;
}

template <class T>
std::vector<unsigned short> SparseSet<T>::getIntersection(std::vector<unsigned short> obj)
{
	auto ids = getIDs();
	std::sort(ids.begin(), ids.end());
	std::sort(obj.begin(), obj.end());
	std::vector<unsigned short> intersection;
	intersection.reserve(std::min(ids.size(), obj.size()));
	std::set_intersection(ids.begin(), ids.end(), obj.begin(), obj.end(), std::back_inserter(intersection));
	return intersection;
}