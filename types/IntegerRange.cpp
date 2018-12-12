#include "stdafx.h"

#include "types/IntegerRange.h"

#include "game/App.h"
#include "utilities/RNGUtils.h"

IntegerRange::IntegerRange()
  :
  m_start{ 0 },
  m_end{ 0 },
  m_uniform{ false }
{}

IntegerRange::IntegerRange(int start, int end, bool uniform)
  :
  m_start{ start },
  m_end{ end },
  m_uniform{ uniform }
{}

IntegerRange::~IntegerRange()
{}

int IntegerRange::pick()
{
  if (m_uniform)
  {
    return the_RNG.pick_uniform(m_start, m_end);
  }
  else
  {
    // Obviously, a "true" normal distribution goes on forever.
    // I want the "start" and "end" values given to be equivalent
    // to 4*sigma for now; later perhaps I will make this a
    // configurable option.
    int range = (m_end - m_start);

    // To map the normal curve to the range I want, mean = 0.5 and
    // sigma = 0.125. This puts 4*sigma at (0.5 - 0.5) to (0.5 + 0.5), or 0 to 1.
    double value = the_RNG.pick_normal(0.5, 0.125);

    // Trim the outliers.
    if (value < 0) value = 0;
    if (value > 1) value = 1;
    // Multiply by the desired range, and add m_start.
    value = (value * range) + m_start;

    return static_cast<int>(value);
  }
}
