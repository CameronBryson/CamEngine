#include "Registry.hpp"

#include "Scripts/Health.hpp"
#include "Scripts/Damage.hpp"

Registry::Registry()
{
    printf("Registry created\n");
    mEntities.reserve(settings::max_entities);
    mFreeIDs.resize(settings::max_entities - 1);
    std::iota(mFreeIDs.begin(), mFreeIDs.end(), 1);

    
}
Registry::~Registry()
{
    //remove all event bindings
    printf("Registry destroyed\n");
}

unsigned short Registry::createEntity()
{
    assert(! mFreeIDs.empty() && "No more entities available.");
    const unsigned short id = mFreeIDs.back();
    mFreeIDs.pop_back();
    mEntities.emplace_back(id);
    return id;
}

void Registry::deleteEntity(unsigned short id)
{
	EventHandler::GetInstance()->commandDispatcher.SendEvent(AddCommandEvent(std::make_unique<DeleteEntityCommand>(mSparseSets, mFreeIDs, mEntities, id)));
    //mCommandQueue.push(std::make_unique<DeleteEntityCommand>(mSparseSets, mFreeIDs, mEntities, id));
}


