#pragma once

#include "stdafx.h"

#include "OwnedByNode.h"

/// The "canvas" that an object's artists draw onto.
class Canvas : public OwnedByNode
{
public:
  Canvas(SceneNode& node);
  virtual ~Canvas();

  IntVec2 size();
  void setSize(IntVec2 size);

  /// If the render texture exists, executes the lambda passed in passing it the texture.
  /// If the texture or lambda do not exist, returns false.
  bool withTexture(std::function<bool (sf::RenderTexture&)> func);

protected:
  /// Destroys any existing texture. Then, if size is not zero, creates a new one.
  void recreateTexture();

private:
  IntVec2 m_size;

  std::unique_ptr<sf::RenderTexture> m_texture;
};
