#include "stdafx.h"

#include "views/EntityCollectionGUIView.h"

EntityCollectionGUIView::EntityCollectionGUIView(sfg::SFGUI& sfgui, std::vector<EntityId> const ids)
  :
  Object({}),
  m_sfgui{ sfgui },
  m_entityIds{ ids }
{}

EntityCollectionGUIView::~EntityCollectionGUIView()
{}

sfg::Table::Ptr EntityCollectionGUIView::getWidget() const
{
  return m_widget;
}

void EntityCollectionGUIView::setWidget(sfg::Table::Ptr widget)
{
  m_widget = widget;
}

std::vector<EntityId> const& EntityCollectionGUIView::getEntityIds() const
{
  return m_entityIds;
}
