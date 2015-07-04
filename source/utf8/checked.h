// Copyright 2006 Nemanja Trifunovic
// Copyright 2015 Henrik S. Gaﬂmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "core.h"

namespace utf8
{
/// The library API - functions intended to be called by the users
inline namespace checked
{
template< typename octet_iterator >
inline octet_iterator append( char32_t cp, octet_iterator result )
{
    using namespace utf8::detail;
    if (!is_code_point_valid( cp ))
    {
        throw invalid_code_point( cp );
    }
    return encode( cp, result );
}

template< typename octet_iterator, typename output_iterator >
output_iterator replace_invalid( octet_iterator begin, octet_iterator end, output_iterator out, char32_t replacement = 0xFFFD )
{
    using namespace utf8::detail;
    typedef octet_iterator iterator_t;

    if (begin > end)
    {
        //TODO: should be an argument exception
        throw not_enough_room( );
    }

    iterator_t tmp;

    while (begin != end)
    {
        // Determine the sequence length based on the lead octet
        typedef typename std::iterator_traits<octet_iterator>::difference_type diff_t;
        if (const diff_t length = sequence_length<diff_t>( *begin ) )
        {
            tmp = begin;
            char32_t cp = ERROR_CHAR;
            switch (length)
            {
            case 1:
                cp = get_sequence<1, err_handler::icp>( begin, end );
                break;
            case 2:
                cp = get_sequence<2, err_handler::icp>( begin, end );
                break;
            case 3:
                cp = get_sequence<3, err_handler::icp>( begin, end );
                break;
            case 4:
                cp = get_sequence<4, err_handler::icp>( begin, end );
                break;
            }
            if (is_code_point_valid( cp ) && length == encoded_utf8_size<diff_t>( cp ))
            {
                do
                {
                    *out++ = *tmp;
                } while (++tmp != begin);
                continue;
            }
        }
        else
        {
            ++begin;
        }
        out = encode( replacement, out );
    }
    return out;
}

template< typename octet_iterator >
char32_t next( octet_iterator &it, octet_iterator end )
{
    using namespace utf8::detail;
    return decode<err_handler::exc>( it, end );
}

template< typename octet_iterator >
char32_t peek_next( octet_iterator it, octet_iterator end )
{
    return next( it, end );
}

/// Deprecated in versions that include "prior"
template< typename octet_iterator >
uint32_t previous( octet_iterator &it, octet_iterator start )
{
    if (it == start)
    {
        throw not_enough_room( );
    }

    octet_iterator end = it;
    // Go back until we hit either a lead octet or start
    while (utf8::detail::is_trail( *--it ))
    {
        if (it == start)
        {
            // error - no lead byte in the sequence
            throw invalid_utf8( static_cast<uint8_t>(*it) );
        }
    }
    return peek_next( it, end );
}

template< typename octet_iterator, typename distance_type >
void advance( octet_iterator &it, distance_type n, octet_iterator end )
{
    for (distance_type i = 0; i < n; ++i)
        next( it, end );
}

template< typename octet_iterator >
typename std::iterator_traits<octet_iterator>::difference_type distance( octet_iterator first, octet_iterator last )
{
    typename std::iterator_traits<octet_iterator>::difference_type dist;
    for (dist = 0; first < last; ++dist)
        next( first, last );
    return dist;
}

template< typename u16bit_iterator, typename octet_iterator >
octet_iterator utf16to8( u16bit_iterator start, u16bit_iterator end, octet_iterator result )
{
    namespace detail = utf8::detail;
    while (start != end)
    {
        char32_t cp = static_cast<char16_t>(*start++);
        // Take care of surrogate pairs first
        if (detail::is_lead_surrogate( cp ))
        {
            if (start != end)
            {
                char16_t trail_surrogate = static_cast<char16_t>(*start++);
                if (detail::is_trail_surrogate( trail_surrogate ))
                {
                    cp = (cp << 10) + trail_surrogate + detail::SURROGATE_OFFSET;
                }
                else
                {
                    throw invalid_utf16( trail_surrogate );
                }
            }
            else
            {
                throw invalid_utf16( static_cast<char16_t>(cp) );
            }
        }
        // Lone trail surrogate
        else if (detail::is_trail_surrogate( cp ))
        {
            throw invalid_utf16( static_cast<char16_t>(cp) );
        }
        result = detail::encode( cp, result );
    }
    return result;
}

template< typename u16bit_iterator, typename octet_iterator >
u16bit_iterator utf8to16( octet_iterator start, octet_iterator end, u16bit_iterator result )
{
    using namespace utf8::detail;

    while (start != end)
    {
        char32_t cp = next( start, end );
        if (cp > 0xffff)
        {
            //make a surrogate pair
            *result++ = static_cast<char16_t>((cp >> 10) + LEAD_OFFSET);
            *result++ = static_cast<char16_t>((cp & 0x3ff) + TRAIL_SURROGATE_MIN);
        }
        else
        {
            *result++ = static_cast<char16_t>(cp);
        }
    }
    return result;
}

template< typename octet_iterator, typename u32bit_iterator >
octet_iterator utf32to8( u32bit_iterator start, u32bit_iterator end, octet_iterator result )
{
    while (start != end)
        result = append( *start++, result );

    return result;
}

template< typename octet_iterator, typename u32bit_iterator >
u32bit_iterator utf8to32( octet_iterator start, octet_iterator end, u32bit_iterator result )
{
    while (start != end)
        *result++ = next( start, end );

    return result;
}

// The iterator class
template< typename octet_iterator >
class iterator : public std::iterator<std::bidirectional_iterator_tag, char32_t>
{
public:
    iterator( )
    {
    }

    iterator( const octet_iterator &octet_it,
        const octet_iterator &range_start,
        const octet_iterator &range_end )
        : it( octet_it )
        , range_start( range_start )
        , range_end( range_end )
    {
        if (it < range_start || it > range_end)
            throw std::out_of_range( "Invalid utf-8 iterator position" );
    }

    // the default "big three" are OK
    octet_iterator base( ) const
    {
        return it;
    }

    char32_t operator *( ) const
    {
        octet_iterator temp = it;
        return next( temp, range_end );
    }

    bool operator ==( const iterator &rhs ) const
    {
        if (range_start != rhs.range_start || range_end != rhs.range_end)
            throw std::logic_error( "Comparing utf-8 iterators defined with different ranges" );
        return (it == rhs.it);
    }

    bool operator !=( const iterator &rhs ) const
    {
        return !(operator ==( rhs ));
    }

    iterator & operator ++( )
    {
        next( it, range_end );
        return *this;
    }

    iterator operator ++( int )
    {
        iterator temp = *this;
        next( it, range_end );
        return temp;
    }

    iterator & operator --( )
    {
        previous( it, range_start );
        return *this;
    }

    iterator operator --( int )
    {
        iterator temp = *this;
        previous( it, range_start );
        return temp;
    }

private:
    octet_iterator it;
    octet_iterator range_start;
    octet_iterator range_end;
}; // class iterator
}
} // namespace utf8
