#ifndef BEATITUDE_H
#define BEATITUDE_H

/// Beatitude represents the holy/unholy status of an object.
/// Unlike NetHack, MetaHack has five Beatitude levels.
/// @todo Decide whether this will actually be used.
enum class Beatitude
{
  Damned = -2,
  Cursed = -1,
  Uncursed = 0,
  Blessed = 1,
  Sacred = 2
};

// Special behavior for ++Beatitude
Beatitude& operator++(Beatitude &original)
{
  int value = static_cast<int>(original);
  value++;
  if (value > 2) value = 2;
  original = static_cast<Beatitude>(value);

  return original;
}

// Special behavior for --Beatitude
Beatitude& operator--(Beatitude &original)
{
  int value = static_cast<int>(original);
  value--;
  if (value < -2) value = -2;
  original = static_cast<Beatitude>(value);

  return original;
}

// Special behavior for Beatitude++
Beatitude operator++(Beatitude &original, int) {
  Beatitude result = original;
  ++original;
  return result;
}

// Special behavior for Beatitude--
Beatitude operator--(Beatitude &original, int) {
  Beatitude result = original;
  ++original;
  return result;
}

#endif // BEATITUDE_H
