#ifndef AISESSILE_H
#define AISESSILE_H

#include "ai/AIStrategy.h"

// Forward declarations
class Thing;

/// The "sessile" AI strategy does nothing at all, and is used for plants,
/// moss, fungi and the like.
class AISessile :
  public AIStrategy
{
  public:
    AISessile(Thing* entity_ptr);
    virtual ~AISessile();

    virtual void execute() override;
  protected:
  private:
};

#endif // AISESSILE_H
