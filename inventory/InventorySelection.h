#pragma once

#include "entity/EntityId.h"
#include "inventory/InventorySlot.h"
#include "Object.h"

// Forward declarations
class Container;
class Entity;

/// InventorySelection is sort of the controller that binds an InventoryView
/// to a Entity to be displayed.
class InventorySelection : public Object
{
  struct EventEntityChanged : public ConcreteEvent<EventEntityChanged>
  {
    EventEntityChanged(EntityId id_)
      :
      entityId(id_)
    {}

    EntityId const entityId;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | id:" << entityId;
    }
  };

  struct EventSelectionChanged : public ConcreteEvent<EventSelectionChanged>
  {
    EventSelectionChanged(std::vector<InventorySlot> slots_, unsigned int quantity_)
      :
      slots{ slots_ },
      quantity{ quantity_ }
    {}

    std::vector<InventorySlot> const slots;
    unsigned int quantity;

    void serialize(std::ostream& os) const
    {
      Event::serialize(os);
      os << " | quantity: " << quantity << " | slots:";
      for (auto& slot : slots)
      {
        os << " " << slot;
      }
    }
  };

public:
  InventorySelection();
  explicit InventorySelection(EntityId entity);
  virtual ~InventorySelection();

  EntityId getViewed() const;
  void setViewed(EntityId entity);

  void toggleSelection(InventorySlot selection);
  size_t getSelectedSlotCount() const;
  std::vector<InventorySlot> const& getSelectedSlots();
  std::vector<EntityId> getSelectedThings();
  void clearSelectedSlots();

  unsigned int getSelectedQuantity() const;
  unsigned int getMaxQuantity() const;
  unsigned int resetSelectedQuantity();
  bool setSelectedQuantity(unsigned int amount);
  bool incSelectedQuantity();
  bool decSelectedQuantity();

  EntityId getEntity(InventorySlot selection);

  /// @todo Not sure if these actually belong here, or somewhere else.
  static char getCharacter(InventorySlot slot);
  static char getCharacter(unsigned int slot_number);
  static InventorySlot getSlot(char character);

protected:

private:
  /// Entity whose contents (or surroundings) are currently being viewed.
  EntityId m_viewed;

  /// Vector of selected inventory slots.
  std::vector< InventorySlot > m_selectedSlots;

  /// Reference to quantity of topmost selected item.
  unsigned int m_selectedQuantity;
};