// Copyright 2015 Henrik Steffen Gaﬂmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <vector>
#include <string>  

#include <boost/noncopyable.hpp>

namespace fixtures
{
struct valid_u8 : virtual boost::noncopyable
{
    const std::string enc_u8 = u8"\u65E5\u0448\U00010346\u0041\U0001D11E\u3044";
    const std::array< size_t, 6 > enc_u8_cp_size = { 3, 2, 4, 1, 4, 3 };
};

struct valid_u16 : virtual boost::noncopyable
{
    const std::u16string enc_u16 = u"\u65E5\u0448\U00010346\u0041\U0001D11E\u3044";
    const std::array< size_t, 6 > enc_u16_cp_size = { 1, 1, 2, 1, 1, 1 };
};

struct valid_u32 : virtual boost::noncopyable
{
    const std::u32string dec = U"\u65E5\u0448\U00010346\u0041\U0001D11E\u3044";;
};

struct valid_u8_with_it : valid_u8
{
    std::string::const_iterator it_u8;

    const std::string::const_iterator enc_u8_beg = enc_u8.cbegin( );
    const std::string::const_iterator enc_u8_end = enc_u8.cend( );
};

struct valid_u16_with_it : valid_u16
{
    std::u16string::const_iterator it_u16;

    const std::u16string::const_iterator enc_u16_beg = enc_u16.cbegin( );
    const std::u16string::const_iterator enc_u16_end = enc_u16.cend( );
};

struct valid_u32_with_it : valid_u32
{
    std::u32string::const_iterator it_u32;

    const std::u32string::const_iterator dec_beg = dec.cbegin( );
    const std::u32string::const_iterator dec_end = dec.cend( );
};

struct invalid_u8 : virtual boost::noncopyable
{
    const std::string enc = "\xe6\x97\xa5\xd1\x88\xFA \x80\xE0\xA0\xC0\xAF\xED\xA0\x80z";
    const std::string exp_res = u8"\u65E5\u0448\uFFFD \uFFFD\uFFFD\uFFFD\uFFFDz";
    const size_t first_invalid_index = 5;
};
}
