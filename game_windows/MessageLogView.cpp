#include "stdafx.h"

#include "game_windows/MessageLogView.h"

#include "config/Settings.h"
#include "game/App.h"
#include "objects/GameLog.h"
#include "types/Color.h"

MessageLogView::MessageLogView(sfg::SFGUI& sfgui,
                               sfg::Desktop& desktop,
                               std::string name,
                               GameLog& model,
                               sf::IntRect dimensions)
  :
  Object({ EventCommandReady::id }),
  m_sfgui{ sfgui },
  m_desktop{ desktop },
  m_model{ model }
{
  // Create a FloatRect out of the IntRect dimensions.
  /// @todo Just pass the FloatRect directly instead.
  sf::FloatRect floatDims = sf::FloatRect(dimensions.left,
                                          dimensions.top,
                                          dimensions.width,
                                          dimensions.height);

  m_window = sfg::Window::Create();
  m_window->SetTitle("Message Log");

  // ==========================================================================
  // The window for showing the message log is going to have the following
  // hierarchy:
  //
  // - Window
  //   - m_layout: Box (VERTICAL)
  //     - m_listbox: ScrolledWindow
  //       - m_listboxLayout: Box (VERTICAL)
  //         - m_labels[n]: Label -- one per message in the buffer
  //     - m_inputbox: EntryPlus

  m_layout = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
  m_listbox = sfg::ScrolledWindow::Create();
  m_listboxLayout = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
  m_inputbox = sfg::EntryPlus::Create();

  // Set the scrolled window's scrollbar policies.
  m_listbox->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC |
                                sfg::ScrolledWindow::VERTICAL_AUTOMATIC);

  // Add the layout box to the scrolled window using a viewport.
  m_listbox->AddWithViewport(m_listboxLayout);

  // Set the scrolled window's minimum size; for X this should be the width of
  // the containing box, minus 2 * spacing.
  m_listbox->SetRequisition({ 80.f, 40.f });

  // Set the input box's minimum size; for X this should be the width of the
  // containing box, minus 2 * spacing.
  m_inputbox->SetRequisition({ 80.f, 20.f });

  // Link the debug input box "key press" signal to a method.
  m_inputbox->GetSignal(sfg::Widget::OnKeyPress)
   .Connect(std::bind(&MessageLogView::handleKeyPressed, this));

  // Link the debug input box "text changed" signal to a method.
  m_inputbox->GetSignal(sfg::Entry::OnTextChanged)
    .Connect(std::bind(&MessageLogView::handleTextChanged, this));

  // Link the window's "resized" event to a method.
  m_window->GetSignal(sfg::Widget::OnSizeAllocate)
    .Connect(std::bind(&MessageLogView::handleWindowResized, this));

  // Add child objects to the view: a scrollable listbox, and an input box.
  m_layout->Pack(m_listbox, true, true);
  m_layout->Pack(m_inputbox, false, true);
  m_layout->SetSpacing(5.f);

  // Add the box to the window, and the window to the desktop.
  m_window->Add(m_layout);
  m_desktop.Add(m_window);

  // Set initial window size. (Has to be done after a widget is added to a hierarchy.)
  m_window->SetAllocation(floatDims);

  // Add an observer to the GameLog model.
  subscribeTo(m_model, EventID::All);
}

MessageLogView::~MessageLogView()
{
  m_model.removeObserver(*this);
}

bool MessageLogView::onEvent(Event const& event)
{
  auto id = event.getId();

  if (id == GameLog::EventMessageAdded::id)
  {
    auto info = static_cast<GameLog::EventMessageAdded const&>(event);
    CLOG(TRACE, "GUI") << "MessageLogView::onEvent(Event const&) fired " << event;
    auto newLabel = sfg::Label::Create();
    newLabel->SetText(info.message);
    newLabel->SetAlignment({ 0.0f, 0.5f });
    m_labels.push_back(newLabel);
    m_listboxLayout->Pack(newLabel, true, true);
  }

  return false;
}

void MessageLogView::handleKeyPressed()
{
  auto key = m_inputbox->GetLastPressedKey();
  if (sf::Keyboard::Enter == key)
  {
    EventCommandReady event(m_inputbox->GetText());
    broadcast(event);
    m_inputbox->SetText("");
  }
}

void MessageLogView::handleTextChanged()
{
  CLOG(TRACE, "GUI") << "MessageLogView::handleTextChanged() fired";
}

void MessageLogView::handleWindowResized()
{
  // auto newSize = m_window->GetAllocation();

  // // Set the scrolled window's minimum size; for X this should be the width of
  // // the containing box, minus 2 * spacing.
  // m_listbox->SetRequisition(sf::Vector2f(newSize.width - 10, 40.f));

  // // Set the input box's minimum size; for X this should be the width of the
  // // containing box, minus 2 * spacing.
  // m_inputbox->SetRequisition(sf::Vector2f(newSize.width - 10, 20.f));
}
