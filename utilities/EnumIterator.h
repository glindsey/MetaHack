#pragma once

#include <iterator>
#include <boost/range/iterator_range.hpp>

/// Implementation of an enum iterator. The enum must define Last as the element
/// following the last "valid" element, and First equal to the first element to
/// iterate over.
/// Adapted from the code at http://coliru.stacked-crooked.com/view?id=bcee5da83cd5c0738a17962bc00ee82d
template<class EnumType>
class EnumIterator
{
private:
  EnumType value;
  typedef typename std::underlying_type<EnumType>::type under;
public:
  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;
  typedef EnumType value_type;
  typedef EnumType reference;
  typedef EnumType* pointer;
  typedef std::random_access_iterator_tag iterator_category;

  constexpr EnumIterator() :value() {}
  constexpr EnumIterator(const EnumIterator& rhs) noexcept(true) :value(rhs.value) {}
  constexpr explicit EnumIterator(EnumType value_) noexcept(true) :value(value_) {}
  ~EnumIterator() noexcept(true) {}
  EnumIterator& operator=(const EnumIterator& rhs) noexcept(true) { value = rhs.valud; return *this; }
  EnumIterator& operator++() noexcept(true) { value = (EnumType)(under(value) + 1); return *this; }
  EnumIterator operator++(int) noexcept(true) { EnumIterator r(*this); ++*this; return r; }
  EnumIterator& operator+=(size_type o) noexcept(true) { value = (EnumType)(under(value) + o); return *this; }
  friend constexpr EnumIterator operator+(const EnumIterator& it, size_type o) noexcept(true) { return EnumIterator((EnumType)(under(it) + o)); }
  friend constexpr EnumIterator operator+(size_type o, const EnumIterator& it) noexcept(true) { return EnumIterator((EnumType)(under(it) + o)); }
  EnumIterator& operator--() noexcept(true) { value = (EnumType)(under(value) - 1); return *this; }
  EnumIterator operator--(int) noexcept(true) { EnumIterator r(*this); --*this; return r; }
  EnumIterator& operator-=(size_type o) noexcept(true) { value = (EnumType)(under(value) + o); return *this; }
  friend constexpr EnumIterator operator-(const EnumIterator& it, size_type o) noexcept(true) { return EnumIterator((EnumType)(under(it) - o)); }
  friend constexpr difference_type operator-(EnumIterator lhs, EnumIterator rhs) noexcept(true) { return under(lhs.value) - under(rhs.value); }
  constexpr reference operator*() const noexcept(true) { return value; }
  constexpr reference operator[](size_type o) const noexcept(true) { return (EnumType)(under(value) + o); }
  constexpr const EnumType* operator->() const noexcept(true) { return &value; }
  constexpr friend bool operator==(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value == rhs.value; }
  constexpr friend bool operator!=(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value != rhs.value; }
  constexpr friend bool operator<(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value < rhs.value; }
  constexpr friend bool operator>(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value > rhs.value; }
  constexpr friend bool operator<=(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value <= rhs.value; }
  constexpr friend bool operator>=(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { return lhs.value >= rhs.value; }
  friend void swap(const EnumIterator& lhs, const EnumIterator& rhs) noexcept(true) { std::swap(lhs.value, rhs.value); }
};
template<class EnumType> constexpr boost::iterator_range<EnumIterator<EnumType>> getRange() noexcept(true)
{
  return boost::make_iterator_range(EnumIterator<EnumType>(EnumType::First), EnumIterator<EnumType>(EnumType::Last));
}