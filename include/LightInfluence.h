#ifndef LIGHTINFLUENCE_H
#define LIGHTINFLUENCE_H

struct LightInfluence
{
  IntegerVec2 coords;  ///< Coordinates of the light.
  sf::Color color;      ///< Color of the light.
  int intensity;        ///< Intensity of the light.
};

#endif // LIGHTINFLUENCE_H
