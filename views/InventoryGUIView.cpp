#include "stdafx.h"

#include "views/InventoryGUIView.h"

InventoryGUIView::InventoryGUIView(sfg::SFGUI& sfgui, EntityId entity)
  :
  Object({}),
  m_sfgui{ sfgui },
  m_entityId{ entity }
{}

InventoryGUIView::~InventoryGUIView()
{}

sfg::Table::Ptr InventoryGUIView::getWidget() const
{
  return m_widget;
}

void InventoryGUIView::setWidget(sfg::Table::Ptr widget)
{
  m_widget = widget;
}

EntityId InventoryGUIView::getEntity() const
{
  return m_entityId;
}
