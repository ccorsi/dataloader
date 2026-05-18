/**
 * @file checkers.h
 * @author Claudio Corsi (clcorsi@yahoo.com)
 * @brief This file contains the different checkers template classes
 * @version 0.1
 * @date 2024-03-04
 *
 * @copyright Copyright (c) 2024 Claudio Corsi
 *
 * [MIT License](https://raw.githubusercontent.com/ccorsi/dataloader/main/LICENSE)
 */

#ifndef __CHECKERS_H__
#define __CHECKERS_H__

#include <istream>
#include <cwctype>
#include <cctype>
#include <type_traits>
#include <stdexcept>

namespace valhalla
{

namespace checkers
{

/**
 * @brief Used in the case that no character are supposed to be skipped.
 *
 * @tparam Type The character stream we are going to use, char or wchar_t.
 */
template<typename Type>
struct is_space_noop {
    /**
     * @brief Simple operator that just returns false.
     *
     * @param chr The character to compare with, not used
     * @return true never returns this
     * @return false this is always returned
     */
    bool operator()(Type chr) { return false; }
};

/**
 * @brief Based implementation that uses the standard isspace or iswspace
 *      methods to determine if a space should be skipped or not.
 *
 * @tparam Type The character stream we are going to use, char or wchar_t.
 * @tparam IntType The value type expected, int or wint_t.
 */
template<typename Type, typename IntType>
struct is_space_base {
    /**
     * @brief Base operator used to determine if the passed character should be
     *  considered as a space or not.
     *
     * @details This base operator will be able to determine if the passed character
     *  is considered to be a space character or not.  It will use the standard 
     *  isspace and and iswspace functions to determine if the passed character is
     *  a space for char and wchar_t character inputs.  It will then throw a runtime
     *  error if the defined Type is not defined as a char or wchar_t input type.
     *
     * @param chr The character that will be checked if it is a space or not
     * @return true If the passed character is considered to be a space
     * @return false If the passed character is not considered to be a space
     */
    bool operator()(IntType chr) {
        if constexpr (std::is_same_v<char, Type>) {
            return std::isspace(chr) != 0;
        } else {
            if constexpr (std::is_same_v<wchar_t, Type>) {
                return std::iswspace(chr) != 0;
            } else {
                // #error "Incorrect character type, only char or wchar_t are valid types "
                throw std::runtime_error(std::string("Unknown character type: ") + std::string(typeid(Type).name()));
            }
        }
    }
};

/**
 * @brief char based space character check.
 *
 * @details This struct is used to determine if each passed character
 *  is considered a space or not for char types of input characters.  This
 *  struct is a specialized version of the is_space_base template class
 *  for char input characters.
 */
struct is_space : public is_space_base<char,int> {
};

/**
 * @brief wchar_t based space character check.
 *
 * @details This struct is used to determine if each passed character
 * is considered a space or not for wchar_t types of input characters.
 * This struct is a specialized version of the is_space_base template
 * class for wchar_t input characters.
 */
struct is_wspace : public is_space_base<wchar_t, wint_t> {
};

/**
 * @brief Variable length string of characters to compare with.
 *
 * @tparam Char The type of characters we are comparing, char or wchar_t
 * @tparam IntType The type of parameter type the operator() expects
 * @tparam Characters A class or struct that contains a static str string field
 */
template<typename Char, typename IntType, typename Characters>
struct skip_characters_base {
    /**
     * @brief Operator used to determine if the passed character should be skipped or not
     *
     * @param chr The character to check
     * @return true If the passed character can be skipped
     * @return false If the passed character can not be skipped
     */
    bool operator()(IntType chr) {
        if constexpr (std::is_same_v<char, Char>) {
            return std::isspace(chr) || Characters::str.find(chr) != Characters::str.npos;
        } else {
            if constexpr (std::is_same_v<wchar_t, Char>) {
                return std::iswspace(chr) || Characters::str.find(chr) != Characters::str.npos;
            } else {
                // #error "Incorrect character type, only char or wchar_t are valid types "
                throw std::runtime_error(std::string("Unknown character type: ") + std::string(typeid(Char).name()));
            }
        }
    }
};

template<typename Characters>
struct skip_characters : skip_characters_base<char, int, Characters> {
};

template<typename Characters>
struct skip_wcharacters : skip_characters_base<wchar_t, wint_t, Characters> {
};

template<typename Characters>
struct skip_chars {
    std::string characters;
    skip_chars() : characters(Characters::str) {}
    bool operator()(int chr) {
        return std::isspace(chr) || characters.find(chr) != characters.npos;
    }
};
/**
 * @brief variable length character check method used to determine if a given
 *      character should be skipped or not.
 *
 * @tparam Type The character stream we are going to use, char or wchar_t.
 * @tparam IntType The value type expected, int or wint_t.
 * @tparam Chars Variable length of characters that should be skipped.
 */
template<typename Type, typename IntType, Type... Chars>
struct is_space_or_base {
    /**
     * @brief Used to determine if the passed chr character is the same as the
     *  passed check character.
     *
     * @param chr The character to check against
     * @param check The character that we are looking for
     * @return true If the two passed characters are the same
     * @return false If the two passed characters are different
     */
    bool any_of(IntType chr, Type check) {
        return chr == check;
    }

    /**
     * @brief Used to determine if the passed chr character is the same as the
     *  passed check character.  If it is, this method will return else it will
     *  call itself recursively comparing the next available character with the
     *  passed chr character.
     *
     * @tparam Rest The type of character that is being passed along
     * @param chr The character to check against
     * @param check The character that we are looking for
     * @param rest An var args of the remaining characters to check against
     * @return true If the passed chr is the same as check or the recursive call successed
     * @return false If the passed chr is different from all of the remaining characters
     */
    template<typename... Rest>
    bool any_of(IntType chr, Type check, Rest... rest) {
        return chr == check || any_of(chr, rest...);
    }

    /**
     * @brief Determines if the passed chr character is part of the list of characters
     *  that it is being compared to.
     *
     * @param chr The character that we are checking for
     * @return true If the passed character is part of the list of characters
     * @return false If the passed character is not part of the list of characters
     */
    bool operator()(IntType chr) {
        if constexpr (std::is_same_v<char, Type>) {
            return std::isspace(chr) || any_of(chr, Chars...);
        } else {
            if constexpr (std::is_same_v<wchar_t, Type>) {
                return std::iswspace(chr) || any_of(chr, Chars...);
            } else {
                // #error "Incorrect character type, only char or wchar_t are valid types "
                throw std::runtime_error(std::string("Unknown character type: ") + std::string(typeid(Type).name()));
            }
        }
    }
};

/**
 * @brief Variable length character skipping for char input stream.
 *
 * @tparam Chars Variable length char characters that should be skipped.
 */
template<char... Chars>
struct is_space_or : public is_space_or_base<char, int,Chars...> {
};

/**
 * @brief Variable length character skipping for wchar_t input stream.
 *
 * @tparam Chars Variable length wchar_t characters that should be skipped.
 */
template<wchar_t... Chars>
struct is_wspace_or : public is_space_or_base<wchar_t, wint_t,Chars...> {
};

/**
 * @brief Used to skip zero or more characters using the passed IsSpace
 *      struct.
 *
 * @tparam Char The character stream we are going to use, char or wchar_t.
 * @tparam IsSpace The space skipping struct used to check if a space should be skipped
 */
template<typename Char, typename IsSpace>
struct skip_spaces {
    IsSpace isSpace;
    std::basic_istream<Char> & operator()(std::basic_istream<Char> & in) {
        while (isSpace(static_cast<Char>(in.peek()))) in.get();
        return in;
    }
};
// This requres c++20 to build
// template<typename Container, Container characters>
// struct skip_chars {
//     bool operator()(int chr) { return std::isspace(chr) || characters.find(chr) != characters.end(); }
// };
/**
 * @brief Used to determine if current character in the input stream is the
 *      expected character.  In this case, it will always return true.
 *
 * @tparam Char The character stream we are going to use, char or wchar_t.
 */
template<typename Char>
struct is_character_noop {
    bool operator()(std::basic_istream<Char> & in) {
        return true;
    }
};

/**
 * @brief Used to determine if the current character in the input stream is the
 *      expected character.  In this case, it will determine if the passed
 *      character if part of the passed variable length Chars.
 *
 * @tparam Char The character stream we are going to use, char or wchar_t.
 * @tparam Chars Variable length of expected characters.
 */
template<typename Char, Char... Chars>
struct is_character {
    bool operator()(std::basic_istream<Char> & in, Char check) {
        if (static_cast<Char>(in.peek()) != check) {
            return false;
        } // if
        in.get(); // read check character
        return true;
    }

    template<typename... Rest>
    bool operator()(std::basic_istream<Char> & in, Char check, Rest... rest) {
        if (static_cast<Char>(in.peek()) != check) {
            return operator()(in, rest...);
        } // if
        in.get(); // read check character
        return true;
    }

    bool operator()(std::basic_istream<Char> & in) {
        return operator()(in, Chars...);
    }
};

/**
 * @brief Used to determine if the current character in the input stream is the
 *      expected character.  In this case, it will return true for every other
 *      characters processed.  This was created to handle a unique issue with
 *      the data loader template class.  Specifically the case in which the
 *      character being checked will not be consumed.
 *
 * @todo confirm that the above statement is true.
 *
 * @tparam Char The character stream we are going to use, char or wchar_t.
 */
template<typename Char>
struct is_no_character {
    int m_count = 0;
    bool operator()(std::basic_istream<Char> & in) {
        m_count = ++m_count % 2;
        return m_count == 0;
    }
};

} // namespace checkers

} // namespace valhalla

#endif // __CHECKERS_H__
