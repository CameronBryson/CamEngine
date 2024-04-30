#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>
using EntityID = std::uint16_t;
using ComponentID = std::uint16_t;
using ArchetypeID = std::uint16_t;
using ArchetypeSet = std::unordered_set<ArchetypeID>; 
using Type = std::vector<ComponentID>;

struct Archetype{
  ArchetypeID id; //unique id for this archetype
  Type type;
};
class Registry{
public:
  template<typename T>
  void CreatePool(){
  }
  template<typename T>
  void ShutdownPool();

private:
  std::unordered_map<EntityID, Archetype> entity_map; //Find what archetype this entity belongs to
  std::unordered_map<ComponentID, ArchetypeSet> component_map; //Find all the archetypes that contain this component
  //std::unordered_map<Type, Archetype> archetype_map;//Find an archetype by its list of components
};

#endif // REGISTRY_HPP
