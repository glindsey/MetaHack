#include "views/EntityCollectionGUIListView.h"

#include "config/Settings.h"
#include "utilities/New.h"
#include "views/EntityView2D.h"
#include "systems/Manager.h"
#include "systems/SystemNarrator.h"

EntityCollectionGUIListView::EntityCollectionGUIListView(sfg::SFGUI& sfgui, std::vector<EntityId> ids)
  :
  EntityCollectionGUIView(sfgui, ids)
{
  auto& narrator = SYSTEMS.narrator();
  auto& config = Config::settings();
  RealVec2 tileSize = config.get("graphics-tile-size");

  auto const fillAndExpand = sfg::Table::FILL | sfg::Table::EXPAND;
  auto const fill = sfg::Table::FILL;
  auto const padding = sf::Vector2f(5.f, 5.f);

  /// @todo Flesh me out
  auto tableWidget = sfg::Table::Create();

  for(unsigned int counter = 0; counter < ids.size(); ++counter)
  {
    auto& id = ids[counter];

    /// @todo These should maybe be their own widgets eventually
    /// Column 0: Icon representation of item
    /// @todo Add the icon representation of the item

    // This is inefficient, to say the least, but for now it will suffice.
    auto canvas = sfg::Canvas::Create();
    auto entityView = std::unique_ptr<EntityView>(NEW EntityView2D(id));
    canvas->SetRequisition(tileSize);
    entityView->setLocation({ 0.0f, 0.0f });
    entityView->setSize(tileSize);
    auto rectangle = entityView->drawRectangle(0);
    canvas->Bind();
    canvas->Draw(rectangle);
    canvas->Display();
    canvas->Unbind();
    tableWidget->Attach(canvas, { 0, counter, 1, 1 }, sfg::Table::EXPAND, sfg::Table::EXPAND, padding);

    /// Column N: Description of item
    auto descLabel = sfg::Label::Create();
    descLabel->SetText(narrator.getDescriptiveString(id, ArticleChoice::Indefinite, UsePossessives::Yes));
    descLabel->SetAlignment({ 0.0f, 0.5f });
    tableWidget->Attach(descLabel, { 1, counter, 1, 1 }, fillAndExpand, fillAndExpand, padding);
  }

  setWidget(tableWidget);
}

EntityCollectionGUIListView::~EntityCollectionGUIListView()
{}

bool EntityCollectionGUIListView::onEvent(Event const& event)
{
  /// @todo STUFF
  return false;
}
