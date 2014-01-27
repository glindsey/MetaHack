#include "ai/AIStrategy.h"

#include "Entity.h"

struct AIStrategy::Impl
{
  Entity* entity_ptr;
};

AIStrategy::AIStrategy(Entity* entity_ptr) :
  impl(new Impl())
{
  impl->entity_ptr = entity_ptr;  //ctor
}

AIStrategy::~AIStrategy()
{
  //dtor
}
