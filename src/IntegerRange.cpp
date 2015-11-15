#include "IntegerRange.h"

#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

#include "App.h"
#include "ErrorHandler.h"

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
    boost::random::uniform_int_distribution<> dist(m_start, m_end);
    return dist(the_RNG);
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
    boost::random::normal_distribution<> dist(0.5, 0.125);
    double value = dist(the_RNG);

    // Trim the outliers.
    if (value < 0) value = 0;
    if (value > 1) value = 1;
    // Multiply by the desired range, and add m_start.
    value = (value * range) + m_start;

    return static_cast<int>(value);
  }
}