#ifndef INVENTORYAREA_H
#define INVENTORYAREA_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

#include "EventHandler.h"
#include "Renderable.h"
#include "ThingId.h"

enum class InventoryType
{
  Inside,
  Around
};

// Forward declarations
class Container;

class InventoryArea :
  public EventHandler,
  public Renderable
{
  public:
    InventoryArea(sf::IntRect dimensions,
                  std::vector<ThingId>& selected_things,
                  unsigned int& selected_quantity);
    virtual ~InventoryArea();

    void set_focus(bool focus);
    bool get_focus();

    void set_capital_letters(bool use_capitals);
    bool get_capital_letters();

    sf::IntRect get_dimensions();
    void set_dimensions(sf::IntRect dimensions);

    Container& get_viewed_container();
    void set_viewed_container(Container& container);

    void toggle_selection(unsigned int selection);

    InventoryType get_inventory_type();
    void set_inventory_type(InventoryType type);

    virtual EventResult handle_event(sf::Event& event);

    virtual bool render(sf::RenderTarget& target, int frame);

  protected:
  private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // STATUSAREA_H
