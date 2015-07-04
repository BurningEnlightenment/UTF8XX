// Copyright 2015 Henrik Steffen Gaﬂmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <boost/test/unit_test.hpp>

#include <utf8.h>

#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE( utf8ut_unchecked )

namespace lib = utf8::unchecked;

struct append_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( append, append_fixture )
{
    it_u8 = enc_u8_beg;
    for (char32_t cp : dec)
    {
        std::string res;
        std::string::const_iterator it2_u8 = it_u8;
        BOOST_TEST_CHECKPOINT( "cp=U+" << std::hex << (uint32_t)cp );
        utf8::unchecked::advance( it_u8, 1 );

        lib::append( cp, std::back_inserter( res ) );
        BOOST_REQUIRE_EQUAL_COLLECTIONS( res.cbegin( ), res.cend( ), it2_u8, it_u8 );

        it2_u8 = lib::append( cp, res.begin( ) );
        BOOST_REQUIRE( it2_u8 == res.cend( ) );
    }
}

struct next_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( next, next_fixture )
{
    it_u8 = enc_u8_beg;
    for (size_t i = 0; i < dec.size( ); ++i)
    {
        std::string::const_iterator last = it_u8;
        BOOST_TEST_CHECKPOINT( "i=" << i << " cp=U+" << std::hex << (uint32_t)dec[i] );

        char32_t cp = lib::next( it_u8 );
        BOOST_CHECK_EQUAL( cp, dec[i] );
        BOOST_REQUIRE( it_u8 == last + enc_u8_cp_size[i] );
    }
}

struct previous_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( previous, previous_fixture )
{
    it_u8 = enc_u8_end;

    for (size_t i = dec.size( ); i > 0; --i)
    {
        std::string::const_iterator last = it_u8;
        BOOST_TEST_CHECKPOINT( "i=" << i << " cp=U+" << std::hex << (uint32_t)dec[i - 1] );

        char32_t cp = lib::previous( it_u8 );
        BOOST_REQUIRE_EQUAL( cp, dec[i - 1] );
        BOOST_REQUIRE( it_u8 == last - enc_u8_cp_size[i - 1] );
    }
}

BOOST_FIXTURE_TEST_CASE( advance, fixtures::valid_u8_with_it )
{
    it_u8 = enc_u8_beg;
    lib::advance( it_u8, 2 );
    BOOST_CHECK( it_u8 == enc_u8_beg + enc_u8_cp_size[0] + enc_u8_cp_size[1] );
}

BOOST_FIXTURE_TEST_CASE( distance, fixtures::valid_u8_with_it )
{
    size_t dist = lib::distance( enc_u8_beg, enc_u8_end );
    BOOST_CHECK_EQUAL( dist, enc_u8_cp_size.size( ) );
}

struct utf32conv_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32_with_it {};

BOOST_FIXTURE_TEST_CASE( utf32to8, utf32conv_fixture )
{
    std::string str;
    lib::utf32to8( dec_beg, dec_end, std::back_inserter( str ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u8_beg, enc_u8_end );

    // check returned iterator validity
    it_u8 = lib::utf32to8( dec_beg, dec_end, str.begin( ) );
    BOOST_CHECK( it_u8 == str.cend( ) );
}

BOOST_FIXTURE_TEST_CASE( utf8to32, utf32conv_fixture )
{
    std::u32string str;
    lib::utf8to32( enc_u8_beg, enc_u8_end, std::back_inserter( str ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), dec_beg, dec_end );

    // check returned iterator validity
    it_u32 = lib::utf8to32( enc_u8_beg, enc_u8_end, str.begin( ) );
    BOOST_CHECK( it_u32 == str.end( ) );
}

struct utf16conv_fixture : fixtures::valid_u8_with_it, fixtures::valid_u16_with_it {};

BOOST_FIXTURE_TEST_CASE( utf16to8, utf16conv_fixture )
{
    std::string str;
    lib::utf16to8( enc_u16_beg, enc_u16_end, std::back_inserter( str ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u8_beg, enc_u8_end );

    it_u8 = lib::utf16to8( enc_u16_beg, enc_u16_end, str.begin( ) );
    BOOST_CHECK( it_u8 == str.end( ) );
}

BOOST_FIXTURE_TEST_CASE( utf8to16, utf16conv_fixture )
{
    std::u16string str;
    lib::utf8to16( enc_u8_beg, enc_u8_end, std::back_inserter( str ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u16_beg, enc_u16_end );

    it_u16 = lib::utf8to16( enc_u8_beg, enc_u8_end, str.begin( ) );
    BOOST_CHECK( it_u16 == str.end( ) );
}

struct iterator_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( iterator, iterator_fixture )
{
    using iterator = lib::iterator<std::string::const_iterator>;
    const size_t num_cps = dec.size( );
    iterator it( enc_u8_beg );
    iterator end( enc_u8_end );
    iterator copied_it = it;
    BOOST_REQUIRE( copied_it == it );
    BOOST_REQUIRE_EQUAL( *it, dec[0] );
    BOOST_REQUIRE_EQUAL( *++it, dec[1] );
    BOOST_REQUIRE_EQUAL( *it++, dec[1] );
    BOOST_REQUIRE_EQUAL( *it, dec[2] );
    BOOST_REQUIRE( copied_it != it );
    for (size_t i = 2; i < num_cps; ++i)
    {
        ++it;
    }
    BOOST_REQUIRE( it == end );
    BOOST_REQUIRE_EQUAL( *--it, dec[num_cps - 1] );
    BOOST_REQUIRE_EQUAL( *it--, dec[num_cps - 1] );
    BOOST_REQUIRE_EQUAL( *it, dec[num_cps - 2] );
    for (size_t i = 2; i < num_cps; ++i)
    {
        --it;
    }
    BOOST_REQUIRE( it == iterator( enc_u8_beg ) );
    BOOST_REQUIRE_EQUAL( *it, dec[0] );
}

BOOST_AUTO_TEST_SUITE_END( )
