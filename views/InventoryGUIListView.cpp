#include "stdafx.h"

#include "views/InventoryGUIListView.h"

InventoryGUIListView::InventoryGUIListView(sfg::SFGUI& sfgui, EntityId entity)
  :
  InventoryGUIView(sfgui, entity)
{
  /// @todo WRITE ME
  auto widget = sfg::Table::Create();

  auto tempLabel = sfg::Label::Create();
  auto const fillAndExpand = sfg::Table::FILL | sfg::Table::EXPAND;
  auto const fill = sfg::Table::FILL;
  auto const padding = sf::Vector2f(5.f, 5.f);
  tempLabel->SetText("InventoryGUIListView for " + std::string(entity));
  tempLabel->SetAlignment({0.5f, 0.5f});
  widget->Attach(tempLabel, {0, 0, 2, 1}, fillAndExpand, fillAndExpand, padding);

  setWidget(widget);
}

InventoryGUIListView::~InventoryGUIListView()
{}

bool InventoryGUIListView::onEvent(Event const& event)
{
  /// @todo STUFF
  return false;
}
