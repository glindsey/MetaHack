#pragma once

#include "AssertHelper.h"
#include "components/modifiers/Base.h"
#include "entity/EntityId.h"
#include "types/DequeMap.h"

#include <boost/optional.hpp>
#include <typeinfo>

#include "json.hpp"
using json = ::nlohmann::json;

namespace Components
{
  namespace Modifiers
  {
    /// The generic interface that concrete Collection instances derive from.
    /// Only really used for iterating through multiple components.
    class Collection
    {
    public:
      virtual ~Collection() = default;

      /// Add a modifier to an entity.
      /// If the entity does not have a modifier array, creates one first.
      /// The modifier is added to the end of the array if it exists.
      virtual void addModifier(EntityId id, Base& modifier) = 0;

      /// Add a modifier to an entity.
      /// If the entity does not have a modifier array, creates one first.
      /// The modifier is added to the *beginning* of the array if it exists.
      virtual void addModifierFirst(EntityId id, Base& modifier) = 0;

      /// Clone a modifier array from one entity to another, if the first entity exists.
      virtual void cloneModifiersIfTheyExist(EntityId first, EntityId second) = 0;

      /// Return whether any modifiers exist for the specified ID.
      virtual bool modifiersExistFor(EntityId id) const = 0;

      /// Update the component for a specific ID depending on the provided JSON.
      ///   * If the JSON is empty (null): do nothing.
      ///   * If the JSON is the string "DELETE" (case sensitive), remove all existing modifier data for that ID.
      ///   * Otherwise, replace the component data with the provided JSON.
      virtual void update(EntityId id, json const& newData) = 0;

      /// Create JSON from the entire modifiers map.
      virtual json toJSON() = 0;

      /// Create JSON from the modifiers for a specific ID.
      virtual json toJSON(EntityId id) = 0;

      /// Remove all modifiers for an entity from this map.
      /// If the entity isn't already present, just returns without doing anything.
      virtual void removeAllModifiers(EntityId id) = 0;

      virtual Collection& operator=(json const& j) = 0;

    };

    /// Represents a collection of arrays of a particular modifier, mapped to the entities that contain it.
    template <typename Modifier>
    class CollectionConcrete final : public Collection
    {
    public:
      CollectionConcrete() = default;
      virtual ~CollectionConcrete() = default;

      virtual void addModifier(EntityId id, Base& modifier) override
      {
        CLOG(WARNING, "Component") << "Components::Modifiers::CollectionConcrete.addModifier not yet implemented";
        /// @todo WRITE ME
        //std::string className = typeid(Modifier).name();
        //CLOG(TRACE, "Component") << "Creating new " << typeid(Modifier).name() << " for ID " << id;
        //m_collection[id] = Modifier();
      }

      virtual void addModifierFirst(EntityId id, Base& modifier) override
      {
        CLOG(WARNING, "Component") << "Components::Modifiers::CollectionConcrete.addModifierFirst not yet implemented";
        /// @todo WRITE ME
        //std::string className = typeid(Modifier).name();
        //CLOG(TRACE, "Component") << "Creating new " << typeid(Modifier).name() << " for ID " << id;
        //m_collection[id] = Modifier();
      }

      /// Clone a value from one key to another, if the first key exists.
      virtual void cloneModifiersIfTheyExist(EntityId first, EntityId second) override
      {
        if (modifiersExistFor(first))
        {
          m_collection[second] = m_collection[first];
        }
      }

      virtual bool modifiersExistFor(EntityId id) const override
      {
        return (m_collection.count(id) != 0ULL);
      }

      std::vector<Modifier>& of(EntityId id)
      {
        Assert("Component", modifiersExistFor(id), "Non-existent modifiers of entity " << id << " requested");
        return m_collection.at(id);
      }

      std::vector<Modifier> const& of(EntityId id) const
      {
        Assert("Component", modifiersExistFor(id), "Non-existent modifiers of entity " << id << " requested");
        return m_collection.at(id);
      }

      virtual void update(EntityId id, json const& newData) override
      {
        CLOG(WARNING, "Component") << "Components::Modifiers::CollectionConcrete.update not yet implemented";
        /// @todo FIX ME
        // if (!newData.is_null())
        // {
        //   if (newData.is_string() && newData.get<std::string>() == "DELETE")
        //   {
        //     removeAllModifiers(id);
        //   }
        //   else
        //   {
        //     // (This looks a little funky but makes sure new component is generated
        //     //  if missing.)
        //     operator[](id) = newData;
        //   }
        // }
      }

      virtual json toJSON() override
      {
        json result = *this;
        return result;
      }

      virtual json toJSON(EntityId id) override
      {
        Assert("Component", modifiersExistFor(id), "Non-existent modifiers of entity " << id << " requested");
        json result = m_collection.at(id);
        return result;
      }

      virtual void removeAllModifiers(EntityId id) override
      {
        m_collection.eraseAll(id);
      }

      std::vector<Modifier>& operator[](EntityId id)
      {
        return m_collection[id];
      }

      virtual Collection& operator=(json const& j) override
      {
        from_json(j, *this);
        return *this;
      }

      /// Get the map itself for iterating through.
      std::unordered_map< EntityId, std::vector<Modifier> >& data()
      {
        return m_collection;
      }

      std::unordered_map< EntityId, std::vector<Modifier> > const& data() const
      {
        return m_collection;
      }

      friend void from_json(json const& j, CollectionConcrete& obj)
      {
        CLOG(WARNING, "Component") << "from_json(json const&, Components::Modifiers::CollectionConcrete&) not yet implemented";
        /// @todo FIX ME
        // obj.m_collection.clear();

        // if (j.is_object())
        // {
        //   for (auto citer = j.cbegin(); citer != j.cend(); ++citer)
        //   {
        //     obj.m_collection[citer.key()] = citer.value();
        //   }
        // }
      }

      friend void to_json(json& j, CollectionConcrete const& obj)
      {
        CLOG(WARNING, "Component") << "to_json(json&, Components::Modifiers::CollectionConcrete const&) not yet implemented";
        /// @todo FIX ME
        // j = json::object();
        // for (auto& pair : obj.m_collection)
        // {
        //   j[pair.first] = pair.second;
        // }
      }

    protected:

    private:
      DequeMap<EntityId, Modifier> m_collection;
    };

  } // end namespace Modifiers
} // end namespace
