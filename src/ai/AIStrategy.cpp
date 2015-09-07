#include "ai/AIStrategy.h"

#include "Thing.h"

struct AIStrategy::Impl
{
  Thing* thing_ptr;
};

AIStrategy::AIStrategy(Thing* thing_ptr) :
  pImpl(new Impl())
{
  pImpl->thing_ptr = thing_ptr;  //ctor
}

AIStrategy::~AIStrategy()
{
  //dtor
}
