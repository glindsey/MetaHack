#ifndef ATTRIBUTESET_H
#define ATTRIBUTESET_H

#include <map>

#include "Attribute.h"

struct AttributeData
{
  int default_value;
  int maximum_value;
};

/// Class representing the attributes an Entity can have.
/// It can also represent modifiers granted by something else.
class AttributeSet
{
  public:
    /// Static array containing attribute constants.
    static std::map<Attribute, AttributeData const> constants;

    AttributeSet();
    virtual ~AttributeSet();

    /// Choose whether negative values are allowed.
    /// For sets of absolute stats, such as an Entity's stats, you don't want
    /// them to go below zero.  However, if the AttributeSet represents RELATIVE
    /// stats, such as modifiers, then you DO want them to go below zero.
    /// Changing this value will not immediately affect stats currently set; it
    /// only changes the behavior of the add() and set_to() methods.
    /// @param okay If true, negative values are allowed.
    void set_negative_okay(bool okay);

    /// Return whether negative values are allowed.
    bool get_negative_okay();

    /// Add a value to an attribute.
    /// If the value is greater than the maximum allowed for that statistic, it
    /// is set to that value.  If negative_okay is false, and the value is less
    /// than zero, it is set to zero.
    void add(Attribute attrib, int new_value);

    /// Set an attribute to a particular value.
    /// If the value is greater than the maximum allowed for that statistic, it
    /// is set to that value.  If negative_okay is false, and the value is less
    /// than zero, it is set to zero.
    void set_to(Attribute attrib, int new_value);

    /// Get the value of an attribute.
    unsigned int get(Attribute attrib);

  protected:
  private:
    std::map<Attribute, int> value;
    bool negative_okay;
};

#endif // ATTRIBUTESET_H
