// Copyright 2015 Henrik Steffen Gaﬂmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#include <boost/test/unit_test.hpp>

#include <utf8.h>

#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE( utf8ut_core )

//TODO: add cases for detail functions

BOOST_FIXTURE_TEST_CASE( find_invalid, fixtures::invalid_u8 )
{
    std::string::const_iterator invalid = utf8::find_invalid( enc.cbegin( ), enc.cend( ) );
    BOOST_CHECK( invalid == enc.cbegin( ) + first_invalid_index );
}

BOOST_FIXTURE_TEST_CASE( starts_with_bom, fixtures::valid_u8_with_it )
{
    unsigned char bom[] = { 0xef, 0xbb, 0xbf };
    BOOST_REQUIRE_EQUAL_COLLECTIONS( utf8::bom, utf8::bom + sizeof( utf8::bom ), bom, bom + sizeof( bom ) );

    BOOST_CHECK( utf8::starts_with_bom( bom, bom + sizeof( bom ) ) );
    BOOST_CHECK( !utf8::starts_with_bom( enc_u8_beg, enc_u8_end ) );
}

BOOST_AUTO_TEST_SUITE_END( )
