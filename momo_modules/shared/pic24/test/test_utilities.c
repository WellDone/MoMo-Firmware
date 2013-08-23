#include "unity.h"
#include "utilities.h"

void setUp(void) {

}
void tearDown(void) {

}
void test_get_2byte_number(void) {
  const char* in = "42";
  TEST_ASSERT_EQUAL_INT( 42, get_2byte_number(in) );
}

void test_atoi_small(void) {
	int16 i;
	TEST_ASSERT_TRUE( atoi_small( "42", &i ) );
	TEST_ASSERT_EQUAL_INT( 42, i );

	TEST_ASSERT_TRUE( atoi_small( "-42", &i ) );
	TEST_ASSERT_EQUAL_INT( -42, i );

	TEST_ASSERT_TRUE( atoi_small( "4242", &i ) );
	TEST_ASSERT_EQUAL_INT( 4242, i );

	i = 0;
	TEST_ASSERT_FALSE( atoi_small( "+42", &i ) ); // This fails in the current implementation.  Should it?
	//TEST_ASSERT_EQUAL_INT( 42, i );
	TEST_ASSERT_EQUAL_INT( 0, i );

	TEST_ASSERT_FALSE( atoi_small( "--42", &i ) );
	TEST_ASSERT_EQUAL_INT( 0, i );

	TEST_ASSERT_FALSE( atoi_small( "4-2", &i ) );
	TEST_ASSERT_EQUAL_INT( 0, i );

	TEST_ASSERT_FALSE( atoi_small( "two", &i ) );
	TEST_ASSERT_EQUAL_INT( 0, i );

	// Test 16-bit overflow
	TEST_ASSERT_FALSE( atoi_small( "99999999", &i ) );
	TEST_ASSERT_EQUAL_INT( 0, i );
}

void test_itoa_small(void) {
	char buf[10];
	int16 value;
	int16 len;
	const char* expected_output;

	
	value = 7;
	expected_output = "7";
	len = itoa_small( buf, sizeof(buf), value );
	TEST_ASSERT_EQUAL_INT( strlen(expected_output), len );
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING( expected_output, buf );

	value = 42;
	expected_output = "42";
	len = itoa_small( buf, sizeof(buf), value );
	TEST_ASSERT_EQUAL_INT( strlen(expected_output), len );
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING( expected_output, buf );

	value = -42;
	expected_output = "-42";
	len = itoa_small( buf, sizeof(buf), value );
	TEST_ASSERT_EQUAL_INT( strlen(expected_output), len );
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING( expected_output, buf );

	value = 4242;
	expected_output = "4242";
	len = itoa_small( buf, sizeof(buf), value );
	TEST_ASSERT_EQUAL_INT( strlen(expected_output), len );
	buf[len] = '\0';
	TEST_ASSERT_EQUAL_STRING( expected_output, buf );

	//TODO: Test overflow
}

void test_to_upper_case(void) {
	TEST_ASSERT_EQUAL_UINT8( 'A', to_upper_case('a') );
	TEST_ASSERT_EQUAL_UINT8( 'A', to_upper_case('A') );
	TEST_ASSERT_EQUAL_UINT8( '-', to_upper_case('-') );
	TEST_ASSERT_EQUAL_UINT8( '.', to_upper_case('.') );

}

/*
void test_sprintf_small(void) {
	char buff[50];
	sprintf_small( buff, sizeof(buff), "There were %d %ss in the %s!  %d ate %d.",
		8,
		"ducks",
		"pond",
		7,
		9 );
	TEST_ASSERT_EQUAL_STRING( buff, "There were 8 ducks in the pond!  7 ate 9." );
}
*/