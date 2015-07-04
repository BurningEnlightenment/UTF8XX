// Copyright 2015 Henrik Steffen Gaßmann
// 
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file ../LICENSE or http://www.boost.org/LICENSE_1_0.txt)
////////////////////////////////////////////////////////////////////////////////
#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

bool init_unit_test( )
{
    framework::master_test_suite( ).p_name.value = "UTF8++ test suite";
    return true;
}
