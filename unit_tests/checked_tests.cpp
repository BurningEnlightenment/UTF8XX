// Copyright 2015 Henrik Steffen Ga�mann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

#include <boost/test/unit_test.hpp>

#include <utf8.h>

#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE( utf8ut_checked )

struct append_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( append, append_fixture )
{
    it_u8 = enc_u8_beg;
    for (char32_t cp : dec)
    {
        std::string res;
        std::string::const_iterator it2_u8 = it_u8;
        BOOST_TEST_CHECKPOINT( "utf8ut_checked::append cp=U+" << std::hex << (uint32_t)cp );
        utf8::unchecked::advance( it_u8, 1 );

        BOOST_REQUIRE_NO_THROW( utf8::append( cp, std::back_inserter( res ) ) );
        BOOST_REQUIRE_EQUAL_COLLECTIONS( res.cbegin( ), res.cend( ), it2_u8, it_u8 );

        BOOST_REQUIRE_NO_THROW( it2_u8 = utf8::append( cp, res.begin( ) ) );
        BOOST_REQUIRE( it2_u8 == res.cend( ) );
    }
}

BOOST_FIXTURE_TEST_CASE( replace_invalid, fixtures::invalid_u8 )
{
    std::string str;
    BOOST_REQUIRE_NO_THROW( utf8::replace_invalid( enc.cbegin( ), enc.cend( ), std::back_inserter( str ) ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), exp_res.cbegin( ), exp_res.end( ) );

    std::string::iterator it;
    BOOST_REQUIRE_NO_THROW( it = utf8::replace_invalid( enc.cbegin( ), enc.cend( ), str.begin( ) ) );
    BOOST_CHECK( it == str.end( ) );
}

struct next_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( next, next_fixture )
{
    it_u8 = enc_u8_beg;
    for (size_t i = 0; i < dec.size( ); ++i)
    {
        BOOST_TEST_CHECKPOINT( "utf8ut_checked::next i=" << i );
        std::string::const_iterator last = it_u8;
        char32_t cp;
        BOOST_REQUIRE_NO_THROW( cp = utf8::next( it_u8, enc_u8_end ) );
        BOOST_CHECK_EQUAL( cp, dec[i] );
        BOOST_REQUIRE( it_u8 == last + enc_u8_cp_size[i] );
    }
}

BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( previous, 1 )
struct previous_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( previous, previous_fixture )
{
    it_u8 = enc_u8_end;

    for (size_t i = dec.size( ); i > 0; --i)
    {
        std::string::const_iterator last = it_u8;
        BOOST_TEST_CHECKPOINT( "utf8ut_checked::prior cp=U+" << std::hex << (uint32_t)dec[i - 1] );
        char32_t cp;
        BOOST_REQUIRE_NO_THROW( cp = utf8::previous( it_u8, enc_u8_beg ) );
        BOOST_REQUIRE_EQUAL( cp, dec[i - 1] );
        BOOST_REQUIRE( it_u8 == last - enc_u8_cp_size[i - 1] );
    }

    BOOST_CHECK( it_u8 == enc_u8_beg );
    BOOST_CHECK_THROW( utf8::previous( it_u8, enc_u8_beg ), utf8::not_enough_room );
    BOOST_CHECK( it_u8 == enc_u8_beg );

    //TODO: provide stronger exception guarantee
    it_u8 = enc_u8_beg + 2;
    BOOST_CHECK_THROW( utf8::previous( it_u8, enc_u8_beg + 1 ), utf8::invalid_utf8 );
    BOOST_CHECK( it_u8 == enc_u8_beg + 2 );
}

//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( advance, 1 )

BOOST_FIXTURE_TEST_CASE( advance, fixtures::valid_u8_with_it )
{
    it_u8 = enc_u8_beg;
    BOOST_REQUIRE_NO_THROW( utf8::advance( it_u8, 2, enc_u8_end ) );
    BOOST_CHECK( it_u8 == enc_u8_beg + enc_u8_cp_size[0] + enc_u8_cp_size[1] );

    // internal::validate_sequence/sequence_length happily iterates over the end
    //it_u8 = enc_u8_beg;
    //BOOST_CHECK_THROW( utf8::advance( it_u8, enc_u8_cp_size.size( ) + 1, enc_u8_end ), utf8::not_enough_room );
    //it_u8 = enc_u8_end + 1;
    //BOOST_CHECK_THROW( utf8::advance( it_u8, 1, enc_u8_end ), utf8::not_enough_room );
}

//BOOST_AUTO_TEST_CASE_EXPECTED_FAILURES( distance, 1 )

BOOST_FIXTURE_TEST_CASE( distance, fixtures::valid_u8_with_it )
{
    size_t dist;
    BOOST_REQUIRE_NO_THROW( dist = utf8::distance( enc_u8_beg, enc_u8_end ) );
    BOOST_CHECK_EQUAL( dist, enc_u8_cp_size.size( ) );
    //BOOST_CHECK_THROW( utf8::distance( enc_u8_beg + 1, enc_u8_beg ), utf8::exception );
}

struct utf32conv_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32_with_it {};

BOOST_FIXTURE_TEST_CASE( utf32to8, utf32conv_fixture )
{
    std::string str;
    BOOST_REQUIRE_NO_THROW( utf8::utf32to8( dec_beg, dec_end, std::back_inserter( str ) ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u8_beg, enc_u8_end );

    // check returned iterator validity
    it_u8 = utf8::utf32to8( dec_beg, dec_end, str.begin( ) );
    BOOST_CHECK( it_u8 == str.cend( ) );
}

BOOST_FIXTURE_TEST_CASE( utf8to32, utf32conv_fixture )
{
    std::u32string str;
    BOOST_REQUIRE_NO_THROW( utf8::utf8to32( enc_u8_beg, enc_u8_end, std::back_inserter( str ) ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), dec_beg, dec_end );

    // check returned iterator validity
    BOOST_REQUIRE_NO_THROW( it_u32 = utf8::utf8to32( enc_u8_beg, enc_u8_end, str.begin( ) ) );
    BOOST_CHECK( it_u32 == str.end( ) );
}

struct utf16conv_fixture : fixtures::valid_u8_with_it, fixtures::valid_u16_with_it {};

BOOST_FIXTURE_TEST_CASE( utf16to8, utf16conv_fixture )
{
    std::string str;
    BOOST_REQUIRE_NO_THROW( utf8::utf16to8( enc_u16_beg, enc_u16_end, std::back_inserter( str ) ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u8_beg, enc_u8_end );

    it_u8 = utf8::utf16to8( enc_u16_beg, enc_u16_end, str.begin( ) );
    BOOST_CHECK( it_u8 == str.end( ) );
}

BOOST_FIXTURE_TEST_CASE( utf8to16, utf16conv_fixture )
{
    std::u16string str;
    BOOST_REQUIRE_NO_THROW( utf8::utf8to16( enc_u8_beg, enc_u8_end, std::back_inserter( str ) ) );
    BOOST_REQUIRE_EQUAL_COLLECTIONS( str.cbegin( ), str.cend( ), enc_u16_beg, enc_u16_end );

    BOOST_REQUIRE_NO_THROW( it_u16 = utf8::utf8to16( enc_u8_beg, enc_u8_end, str.begin( ) ) );
    BOOST_CHECK( it_u16 == str.end( ) );
}

struct iterator_fixture : fixtures::valid_u8_with_it, fixtures::valid_u32 {};

BOOST_FIXTURE_TEST_CASE( iterator, iterator_fixture )
{
    using iterator = utf8::iterator<std::string::const_iterator>;
    const size_t num_cps = dec.size( );
    iterator it( enc_u8_beg, enc_u8_beg, enc_u8_end );
    iterator end( enc_u8_end, enc_u8_beg, enc_u8_end );
    iterator copied_it = it;
    BOOST_REQUIRE( copied_it == it );
    BOOST_REQUIRE_EQUAL( *it, dec[0] );
    BOOST_REQUIRE_EQUAL( *++it, dec[1] );
    BOOST_REQUIRE_EQUAL( *it++, dec[1] );
    BOOST_REQUIRE_EQUAL( *it, dec[2] );
    BOOST_REQUIRE( copied_it != it );
    for (size_t i = 2; i < num_cps; ++i )
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
    BOOST_REQUIRE( it == iterator( enc_u8_beg, enc_u8_beg, enc_u8_end ) );
    BOOST_REQUIRE_EQUAL( *it, dec[0] );
}

BOOST_AUTO_TEST_SUITE_END( )
