// Copyright 2015 Henrik Steffen Gaßmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#include <array>

#include <boost/noncopyable.hpp>
#include <boost/test/unit_test.hpp>

#include <utf8.h>

BOOST_AUTO_TEST_SUITE( utf8_negative )

struct valid_utf8_decoding_fixture : boost::noncopyable
{
    const std::string encoded = u8"\u03BA\u1F79\u03C3\u03BC\u03B5";
    const std::u32string decoded = U"\u03BA\u1F79\u03C3\u03BC\u03B5";
};

struct first_possible_sequences_of_certain_length_fixture : boost::noncopyable
{
    static const int cases = 4;
    const std::array< const std::string, cases > encoded = {
        std::string( u8"\U00000000", 1 ),
        u8"\U00000080",
        u8"\U00000800",
        u8"\U00010000",
    };
    const std::array< const char32_t, cases > decoded = {
        U'\U00000000',
        U'\U00000080',
        U'\U00000800',
        U'\U00010000',
    };

    first_possible_sequences_of_certain_length_fixture( )
    {
    }
};

struct last_possible_sequences_of_certain_length_fixture : boost::noncopyable
{
    static const int cases = 4;
    const std::array< const std::string, cases > encoded = {
        u8"\U0000007F",
        u8"\U000007FF",
        u8"\U0000FFFF",
        u8"\U0010FFFF",
    };
    const std::array< const char32_t, cases > decoded = {
        U'\U0000007F',
        U'\U000007FF',
        U'\U0000FFFF',
        U'\U0010FFFF',
    };

    last_possible_sequences_of_certain_length_fixture( )
    {
    }
};

struct misc_boundary_conditions_fixture : boost::noncopyable
{
    static const int cases = 4;
    const std::array< const std::string, cases > encoded = {
        u8"\U0000D7FF",
        u8"\U0000E000",
        u8"\U0000FFFD",
        u8"\U0010FFFF",
    };
    const std::array< const char32_t, cases > decoded = {
        U'\U0000D7FF',
        U'\U0000E000',
        U'\U0000FFFD',
        U'\U0010FFFF',
    };

    misc_boundary_conditions_fixture( )
    {
    }
};

struct unexpected_continuation_bytes_fixtures : boost::noncopyable
{
    const std::array< const std::string, 8 > cases = {
        "\x80",
        "\xBF",
        "\x80\xBF",
        "\x80\xBF\x80",
        "\x80\xBF\x80\xBF",
        "\x80\xBF\x80\xBF\x80",
        "\x80\xBF\x80\xBF\x80\xBF",
        "\x80\xBF\x80\xBF\x80\xBF\x80",
    };
    const std::u32string refstring = U"\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD\uFFFD";
    unexpected_continuation_bytes_fixtures( )
    {
    }
};

////////////////////////////////////////////////////////////////////////////////
// checked api
BOOST_AUTO_TEST_SUITE( checked )

BOOST_FIXTURE_TEST_CASE( valid_utf8_decoding, valid_utf8_decoding_fixture )
{
    std::u32string str;

    BOOST_TEST_CHECKPOINT( "valid checked utf8 to 32 conversion" );
    utf8::utf8to32( encoded.cbegin( ), encoded.cend( ), std::back_inserter( str ) );

    BOOST_CHECK_EQUAL_COLLECTIONS(
        str.cbegin( ), str.cend( ), decoded.cbegin( ), decoded.cend( )
        );
}

BOOST_FIXTURE_TEST_CASE( first_possible_sequences_of_certain_length, first_possible_sequences_of_certain_length_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "first_possible_sequences_of_certain_length i=" << i );
        char32_t dec_char = utf8::next( it, end );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_FIXTURE_TEST_CASE( last_possible_sequences_of_certain_length, last_possible_sequences_of_certain_length_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "last_possible_sequences_of_certain_length i=" << i );
        char32_t dec_char = utf8::next( it, end );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_FIXTURE_TEST_CASE( misc_boundary_conditions, misc_boundary_conditions_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "misc_boundary_conditions i=" << i );
        char32_t dec_char = utf8::next( it, end );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_FIXTURE_TEST_CASE( unexpected_continuation_bytes, unexpected_continuation_bytes_fixtures )
{
    for (int i = 0; i < cases.size( ); ++i)
    {
        std::string::const_iterator
            it = cases[i].cbegin( ),
            end = cases[i].cend( );

        const size_t output_size = cases[i].size( );
        std::string output;

        utf8::replace_invalid( it, end, std::back_inserter( output ) );

        std::u32string u32output;
        // another test case covers the functionality of utf8to32, so we can
        // assume it works
        utf8::utf8to32( output.begin( ), output.end( ), std::back_inserter( u32output ) );

        BOOST_REQUIRE_EQUAL_COLLECTIONS( u32output.begin( ), u32output.end( ), refstring.cbegin( ), refstring.cbegin( ) + output_size );
    }
}

BOOST_AUTO_TEST_SUITE_END( )
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// checked api
BOOST_AUTO_TEST_SUITE( unchecked )

BOOST_FIXTURE_TEST_CASE( valid_utf8_decoding, valid_utf8_decoding_fixture )
{
    std::u32string str;

    BOOST_TEST_CHECKPOINT( "valid checked utf8 to 32 conversion" );
    utf8::unchecked::utf8to32( encoded.cbegin( ), encoded.cend( ), std::back_inserter( str ) );

    BOOST_CHECK_EQUAL_COLLECTIONS(
        decoded.cbegin( ), decoded.cend( ), str.begin( ), str.end( )
        );
}

BOOST_FIXTURE_TEST_CASE( first_possible_sequences_of_certain_length, first_possible_sequences_of_certain_length_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "first_possible_sequences_of_certain_length i=" << i );
        char32_t dec_char = utf8::unchecked::next( it );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_FIXTURE_TEST_CASE( last_possible_sequences_of_certain_length, last_possible_sequences_of_certain_length_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "last_possible_sequences_of_certain_length i=" << i );
        char32_t dec_char = utf8::unchecked::next( it );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_FIXTURE_TEST_CASE( misc_boundary_conditions, misc_boundary_conditions_fixture )
{
    for (int i = 0; i < cases; ++i)
    {
        std::string::const_iterator
            it = encoded[i].cbegin( ),
            end = encoded[i].cend( );

        BOOST_CHECKPOINT( "misc_boundary_conditions i=" << i );
        char32_t dec_char = utf8::unchecked::next( it );
        BOOST_CHECK_EQUAL( dec_char, decoded[i] );
    }
}

BOOST_AUTO_TEST_SUITE_END( )
//
////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END( )

