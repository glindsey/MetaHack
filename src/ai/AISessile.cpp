#include "ai/AISessile.h"

#include "Thing.h"

AISessile::AISessile(Thing* entity_ptr)
  : AIStrategy(entity_ptr)
{
  //ctor
}

AISessile::~AISessile()
{
  //dtor
}

void AISessile::execute()
{
  // Dumbest AI strategy of all, in that there is none.
}
