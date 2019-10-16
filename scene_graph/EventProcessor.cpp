#include "stdafx.h"

#include "EventProcessor.h"

#include "utilities/New.h"

EventProcessor::EventProcessor(SceneNode& node) :
  OwnedByNode(node)
{

}

EventProcessor::~EventProcessor()
{

}
