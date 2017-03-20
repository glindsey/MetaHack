#ifndef INTEGERRANGE_H
#define INTEGERRANGE_H

/// A class that represents a possible range of integers from which to pick a
/// random result.
/// @todo See if this will still be used.
class IntegerRange
{
public:
  IntegerRange();
  IntegerRange(int start, int end, bool uniform = false);
  ~IntegerRange();

  int pick();

private:
  int m_start;
  int m_end;
  bool m_uniform;
};

#endif // INTEGERRANGE_H
