#pragma once

// C++ STL
#include <memory>

// HOP
#include <hop_bits/helper/printable.hpp>

namespace hop {
  // Returns the string representation of a class marked as printable;
  std::string to_string(
      // The class marked as printable.
      const Printable& printable);
  std::string to_string(
      // A shared pointer to the class marked as printable.
      const std::shared_ptr<Printable> printable);

  // Checks whether the provided text ends with another string. This is usually used in this library
  // to check/filter for file endings.
  bool endsWith(
      // The text to be checked.
      const std::string& text,
      // The suffix searched for.
      const std::string& ending);
}
