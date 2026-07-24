#include "color.h"

#include <iomanip>
#include <sstream>

namespace observe
{
  std::string Color::encode() const
  {
    std::ostringstream stream;
    encode(stream);
    return stream.str();
  }

  void Color::encode(std::ostringstream &stream) const
  {
    stream << std::setprecision(std::numeric_limits<float>::max_digits10);

    for (auto value : values_)
      stream << value << " ";

    stream << hdr_ << std::endl;
  }

  void Color::decode(const std::string &data)
  {
    std::istringstream stream(data);
    decode(stream);
  }

  void Color::decode(std::istringstream &stream)
  {
    for (auto &value : values_)
      stream >> value;

    stream >> hdr_;
  }
}