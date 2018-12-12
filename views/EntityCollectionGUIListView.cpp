#include "stdafx.h"

#include "views/EntityCollectionGUIListView.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"

EntityCollectionGUIListView::EntityCollectionGUIListView(sfg::SFGUI& sfgui, std::vector<EntityId> ids)
  :
  EntityCollectionGUIView(sfgui, ids)
{
  auto& narrator = SYSTEMS.narrator();

  auto const fillAndExpand = sfg::Table::FILL | sfg::Table::EXPAND;
  auto const fill = sfg::Table::FILL;
  auto const padding = sf::Vector2f(5.f, 5.f);

  /// @todo Flesh me out
  auto widget = sfg::Table::Create();

  unsigned int counter = 0;
  for(auto iter = ids.cbegin(); iter != ids.cend(); ++iter, ++counter)
  {
    /// @todo These should maybe be their own widgets eventually
    /// Column 0: Icon representation of item
    /// @todo Add the icon representation of the item

    /// Column N: Description of item
    auto descLabel = sfg::Label::Create();
    descLabel->SetText(narrator.getDescriptiveString(*iter, ArticleChoice::Indefinite, UsePossessives::Yes));
    descLabel->SetAlignment({ 0.0f, 0.5f });
    widget->Attach(descLabel, { 1, counter, 1, 1 }, fillAndExpand, fillAndExpand, padding);
  }

  setWidget(widget);
}

EntityCollectionGUIListView::~EntityCollectionGUIListView()
{}

bool EntityCollectionGUIListView::onEvent(Event const& event)
{
  /// @todo STUFF
  return false;
}
