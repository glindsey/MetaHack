#ifndef STRINGABLE_H_INCLUDED
#define STRINGABLE_H_INCLUDED


/**
 * @brief      Interface for a class that has a "to_string" function available.
 */
class Stringable
{
public:
    std::string to_string() = 0;

    std::ostream& operator<<(std::ostream &os)
    {
        return os << to_string();
    }
}

#endif // STRINGABLE_H_INCLUDED