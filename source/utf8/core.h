// Copyright 2006 Nemanja Trifunovic
// Copyright 2015 Henrik S. Gaﬂmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace utf8
{
// Base for the exceptions that may be thrown from the library
class exception : public ::std::exception
{
};

// Exceptions that may be thrown from the library functions.
class invalid_code_point : public exception
{
public:
    invalid_code_point( char32_t cp )
        : cp( cp )
    {
    }

    virtual const char * what( ) const noexcept
    {
        return "Invalid code point";
    }

    char32_t code_point( ) const
    {
        return cp;
    }

private:
    char32_t cp;
};

class invalid_utf8 : public exception
{
public:
    invalid_utf8( uint8_t u ) : u8( u )
    {
    }

    virtual const char * what( ) const noexcept
    {
        return "Invalid UTF-8";
    }

    uint8_t utf8_octet( ) const
    {
        return u8;
    }
private:
    uint8_t u8;
};

class invalid_utf16 : public exception
{
public:
    invalid_utf16( uint16_t u )
        : u16( u )
    {
    }

    virtual const char * what( ) const noexcept
    {
        return "Invalid UTF-16";
    }

    uint16_t utf16_word( ) const
    {
        return u16;
    }
private:
    uint16_t u16;
};

class not_enough_room : public exception
{
public:
    virtual const char * what( ) const noexcept
    {
        return "Not enough space";
    }
};

// Helper code - not intended to be directly called by the library users. May be changed at any time
namespace detail
{
// Unicode constants
// Leading (high) surrogates: 0xd800 - 0xdbff
// Trailing (low) surrogates: 0xdc00 - 0xdfff
const uint16_t LEAD_SURROGATE_MIN = 0xD800u;
const uint16_t LEAD_SURROGATE_MAX = 0xDBFFu;
const uint16_t TRAIL_SURROGATE_MIN = 0xDC00u;
const uint16_t TRAIL_SURROGATE_MAX = 0xDFFFu;
const uint16_t LEAD_OFFSET = LEAD_SURROGATE_MIN - (0x10000 >> 10);
const uint32_t SURROGATE_OFFSET = 0x10000u - (LEAD_SURROGATE_MIN << 10) - TRAIL_SURROGATE_MIN
const uint32_t CODE_POINT_MAX = 0x10FFFFu
const char32_t ERROR_CHAR = 0xFFFFFFFFu;

template< typename result_type, uintmax_t bit_mask, typename input_type >
constexpr result_type masked_cast( input_type input ) noexcept
{
    return static_cast<result_type>(input & bit_mask);
}

constexpr bool is_trail( uint8_t oc ) noexcept
{
    return oc >> 6 == 0x2;
}

constexpr bool is_lead_surrogate( char32_t cu ) noexcept
{
    return cu >= LEAD_SURROGATE_MIN && cu <= LEAD_SURROGATE_MAX;
}

constexpr bool is_trail_surrogate( char32_t cu ) noexcept
{
    return cu >= TRAIL_SURROGATE_MIN && cu <= TRAIL_SURROGATE_MAX;
}

constexpr bool is_surrogate( char32_t cu ) noexcept
{
    return cu >= LEAD_SURROGATE_MIN && cu <= TRAIL_SURROGATE_MAX;
}

constexpr bool is_code_point_valid( char32_t cp ) noexcept
{
    return cp <= CODE_POINT_MAX && !is_surrogate( cp );
}

template< typename diff_type >
inline diff_type sequence_length( uint8_t lead_byte ) noexcept
{
    if (lead_byte < 0x80)
        return 1;
    else if ((lead_byte >> 5) == 0x6)
        return 2;
    else if ((lead_byte >> 4) == 0xe)
        return 3;
    else if ((lead_byte >> 3) == 0x1e)
        return 4;
    else
        return 0;
}

template <typename octet_difference_type>
inline octet_difference_type encoded_utf8_size( char32_t cp ) noexcept
{
    if (cp < 0x80)
        return 1;
    if (cp < 0x800)
        return 2;
    if (cp < 0x10000)
        return 3;
    if (cp <= CODE_POINT_MAX)
        return 4;

    return 0;
}

template< typename octet_iterator >
inline octet_iterator encode( char32_t cp, octet_iterator result )
{
    if (cp < 0x80)
    {
        *result = static_cast<uint8_t>(cp);
    }
    else if (cp < 0x800)
    {
        *result = static_cast<uint8_t>(cp >> 6 | 0xc0);
        *++result = static_cast<uint8_t>(cp & 0x3f | 0x80);
    }
    else if (cp < 0x10000)
    {
        *result = static_cast<uint8_t>(cp >> 12 | 0xe0);
        *++result = static_cast<uint8_t>(cp >> 6 & 0x3f | 0x80);
        *++result = static_cast<uint8_t>(cp & 0x3f | 0x80);
    }
    else
    {
        *result = static_cast<uint8_t>(cp >> 18 | 0xf0);
        *++result = static_cast<uint8_t>(cp >> 12 & 0x3f | 0x80);
        *++result = static_cast<uint8_t>(cp >> 6 & 0x3f | 0x80);
        *++result = static_cast<uint8_t>(cp & 0x3f | 0x80);
    }
    return ++result;
}

enum class err_handler
{
    // no checks
    none,
    // return invalid codepoint
    icp,
    // throw exception
    exc,
};

template< int seq_length, err_handler eh, typename octet_iterator >
inline char32_t get_sequence( octet_iterator &it, octet_iterator end )
{
    static_assert(0 < seq_length && seq_length < 5, "utf8 sequences are 1 to 4 bytes long");
    // the first byte has to be readable, otherwise you could not know the length

    if (seq_length == 1)
    {
        return masked_cast<char32_t, 0x7F>(*it++);
    }
    else
    {
        char32_t cp = masked_cast<char32_t, (0x7F >> seq_length)>(*it++);
        cp <<= (seq_length - 1) * 6;
        for (int i = seq_length - 1; i != 0; ++it)
        {
            if (eh != err_handler::none && it == end)
            {
                if (eh == err_handler::exc)
                {
                    throw not_enough_room( );
                }
                else
                {
                    return ERROR_CHAR;
                }
            }
            unsigned char tmp = *it;
            if (eh != err_handler::none && !is_trail( tmp ))
            {
                if (eh == err_handler::exc)
                {
                    throw invalid_utf8( *it );
                }
                else
                {
                    return ERROR_CHAR;
                }
            }
            cp |= masked_cast<char32_t, 0x3F>(tmp) << --i * 6;
        }
        return cp;
    }
}

template< err_handler eh, typename octet_iterator >
inline char32_t decode( octet_iterator &it, octet_iterator end )
{
    typedef octet_iterator iterator_t;
    if (eh != err_handler::none && it >= end)
    {
        if (eh == err_handler::exc)
        {
            throw not_enough_room( );
        }
        else
        {
            return ERROR_CHAR;
        }
    }

    // Save the original value of it so we can go back in case of failure
    // Of course, it does not make much sense with i.e. stream iterators
    iterator_t original_it = it;

    // Determine the sequence length based on the lead octet
    typedef typename std::iterator_traits<iterator_t>::difference_type diff_t;
    const diff_t length = sequence_length<diff_t>( *it );

    // calculate the code point
    char32_t cp = ERROR_CHAR;
    try
    {
        switch (length)
        {
        case 1:
            cp = get_sequence<1, eh>( it, end );
            break;
        case 2:
            cp = get_sequence<2, eh>( it, end );
            break;
        case 3:
            cp = get_sequence<3, eh>( it, end );
            break;
        case 4:
            cp = get_sequence<4, eh>( it, end );
            break;
        default:
            if (eh == err_handler::exc)
                throw invalid_utf8( *it );
            else
                return ERROR_CHAR;
        }
    }
    catch (const exception &)
    {
        it = original_it;
        throw;
    }

    if (eh != err_handler::none && !is_code_point_valid( cp ))
    {
        if (eh == err_handler::exc)
        {
            it = original_it;
            throw invalid_code_point( cp );
        }
        else
        {
            return ERROR_CHAR;
        }
    }
    if (eh != err_handler::none && length != encoded_utf8_size<diff_t>( cp ))
    {
        if (eh == err_handler::exc)
        {
            it = original_it;
            throw invalid_utf8( *it );
        }
        else
        {
            return ERROR_CHAR;
        }
    }
    return cp;
}
} // namespace detail

/// The library API - functions intended to be called by the users

// Byte order mark
const uint8_t bom[] = { 0xEF, 0xBB, 0xBF };

template< typename octet_iterator >
octet_iterator find_invalid( octet_iterator it, octet_iterator end )
{
    using namespace detail;
    octet_iterator result;
    do
    {
        result = it;
    } while (decode<err_handler::icp>( it, end ) != ERROR_CHAR);
    return result;
}

template< typename octet_iterator >
inline bool is_valid( octet_iterator start, octet_iterator end )
{
    return find_invalid( start, end ) == end;
}

template< typename octet_iterator >
inline bool starts_with_bom( octet_iterator it, octet_iterator end )
{
    return (it != end && static_cast<uint8_t>(*it) == bom[0])
        && (++it != end && static_cast<uint8_t>(*it) == bom[1])
        && (++it != end && static_cast<uint8_t>(*it) == bom[2]);
}
} // namespace utf8
