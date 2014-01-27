#ifndef INVENTORYAREA_H
#define INVENTORYAREA_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "gui/GUIPane.h"
#include "ThingId.h"

enum class InventoryType
{
  Inside,
  Around
};

// Forward declarations
class Container;

class InventoryArea :
  public GUIPane
{
  public:
    InventoryArea(sf::IntRect dimensions,
                  std::vector<ThingId>& selected_things,
                  unsigned int& selected_quantity);
    virtual ~InventoryArea();

    void set_capital_letters(bool use_capitals);
    bool get_capital_letters();

    Container& get_viewed_container();
    void set_viewed_container(Container& container);

    void toggle_selection(unsigned int selection);

    ThingId get_thingid(unsigned int selection);

    InventoryType get_inventory_type();
    void set_inventory_type(InventoryType type);

    virtual EventResult handle_event(sf::Event& event) override;

  protected:
    virtual std::string render_contents(int frame) override;

  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // STATUSAREA_H
