#include "stdafx.h"

#include "Artist.h"

#include "utilities/New.h"

Artist::Artist(SceneNode& node) :
  OwnedByNode(node)
{

}

Artist::~Artist()
{

}

bool Artist::drawOn(Canvas& canvas)
{
  /// \todo Implement this default method.
  return false;
}
