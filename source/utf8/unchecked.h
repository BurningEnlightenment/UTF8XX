// Copyright 2006 Nemanja Trifunovic
// Copyright 2015 Henrik S. Ga�mann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "core.h"

namespace utf8
{
namespace unchecked
{
template< typename octet_iterator >
inline octet_iterator append( char32_t cp, octet_iterator result )
{
    return utf8::detail::encode(cp, result);
}

template< typename octet_iterator >
inline char32_t next( octet_iterator &it )
{
    typedef octet_iterator iterator_t;
    using namespace utf8::detail;
    switch (sequence_length<int>( *it ))
    {
    case 1:
        return get_sequence<1, err_handler::none>( it, iterator_t( ) );
    case 2:
        return get_sequence<2, err_handler::none>( it, iterator_t( ) );
    case 3:
        return get_sequence<3, err_handler::none>( it, iterator_t( ) );
    case 4:
        return get_sequence<4, err_handler::none>( it, iterator_t( ) );
    default:
        return ERROR_CHAR;
    }
}

template< typename octet_iterator >
inline uint32_t peek_next( octet_iterator it )
{
    return utf8::unchecked::next( it );
}

template< typename octet_iterator >
inline uint32_t previous( octet_iterator &it )
{
    while (detail::is_trail( *--it ))
    {
    }
    return peek_next( it );
}

template< typename octet_iterator, typename distance_type >
void advance( octet_iterator &it, distance_type n )
{
    for (distance_type i = 0; i < n; ++i)
        utf8::unchecked::next( it );
}

template< typename octet_iterator >
typename std::iterator_traits<octet_iterator>::difference_type distance( octet_iterator first, octet_iterator last )
{
    typename std::iterator_traits<octet_iterator>::difference_type dist;
    for (dist = 0; first < last; ++dist)
        utf8::unchecked::next( first );
    return dist;
}

template< typename u16bit_iterator, typename octet_iterator >
octet_iterator utf16to8( u16bit_iterator start, u16bit_iterator end, octet_iterator result )
{
    using detail::masked_cast;
    while (start != end)
    {
        char32_t cp = detail::masked_cast<char32_t, 0xFFFF>(*start++);
        // Take care of surrogate pairs first
        if (detail::is_lead_surrogate( cp ))
        {
            char32_t trail_surrogate = masked_cast<char32_t, 0xFFFF>(*start++);
            cp = (cp << 10) + trail_surrogate + detail::SURROGATE_OFFSET;
        }
        result = append( cp, result );
    }
    return result;
}

template< typename u16bit_iterator, typename octet_iterator >
u16bit_iterator utf8to16( octet_iterator start, octet_iterator end, u16bit_iterator result )
{
    while (start < end)
    {
        uint32_t cp = utf8::unchecked::next( start );
        if (cp > 0xffff)
        { //make a surrogate pair
            *result++ = static_cast<char16_t>((cp >> 10) + detail::LEAD_OFFSET);
            *result++ = static_cast<char16_t>((cp & 0x3ff) + detail::TRAIL_SURROGATE_MIN);
        }
        else
            *result++ = static_cast<char16_t>(cp);
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
    while (start < end)
        *result++ = utf8::unchecked::next( start );

    return result;
}

// The iterator class
template< typename octet_iterator >
class iterator : public std::iterator<std::bidirectional_iterator_tag, uint32_t>
{
    octet_iterator it;
public:
    iterator( )
    {
    }

    explicit iterator( const octet_iterator &octet_it ) : it( octet_it )
    {
    }

    // the default "big three" are OK
    octet_iterator base( ) const
    {
        return it;
    }

    uint32_t operator *( ) const
    {
        return peek_next( it );
    }

    bool operator ==( const iterator &rhs ) const
    {
        return (it == rhs.it);
    }

    bool operator !=( const iterator &rhs ) const
    {
        return !(operator ==( rhs ));
    }

    iterator & operator ++( )
    {
        ::std::advance( it, detail::sequence_length<difference_type>( *it ) );
        return *this;
    }

    iterator operator ++( int )
    {
        iterator temp = *this;
        ::std::advance( it, detail::sequence_length<difference_type>( *it ) );
        return temp;
    }

    iterator & operator --( )
    {
        previous( it );
        return *this;
    }

    iterator operator --( int )
    {
        iterator temp = *this;
        previous( it );
        return temp;
    }
}; // class iterator
} // namespace utf8::unchecked
} // namespace utf8 
