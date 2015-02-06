//Name: test_rtcc
//Type: module
//Sources:../../portable

#include "rtcc.h"
#include "task_manager.h"
#include "unity.h"
#include <stdio.h>

rtcc_datetime date;

void asm_enable_rtcon_write()
{

}

int taskloop_add(task_callback task, void *argument)
{
	return 0;
}

void set_sosc_status(int status)
{

}

void setUp()
{

	date.year = 0;
    date.month = 1;
    date.day = 1;
    date.hours = 0;
    date.minutes = 0;
    date.seconds = 0;
    date.weekday = 0;
}

void tearDown()
{

}

void test_rtcc_timestamp()
{
	rtcc_timestamp stamp;

	stamp = rtcc_create_timestamp(&date);
	TEST_ASSERT_EQUAL_UINT32(0, stamp);
}

void do_month_eval(unsigned int month)
{
	unsigned int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	unsigned int day;
	rtcc_timestamp stamp;

	//Add in leap year correction
	if ((date.year % 4) == 0)
		days[2] = 29;

	date.day = 1;
	stamp = rtcc_create_timestamp(&date);

	for (day=1; day <=days[month]; ++day)
	{
		rtcc_timestamp new_stamp;

		date.day = day;

		new_stamp = rtcc_create_timestamp(&date);
		TEST_ASSERT_EQUAL_UINT32(stamp, new_stamp);
		TEST_ASSERT_EQUAL_UINT16(0, rtcc_timestamp_difference(stamp, new_stamp, NULL));

		stamp += (24UL*60UL*60UL);
	}
}

void do_year_eval(unsigned int year)
{
	unsigned int month;

	date.year = year;

	for (month = 1; month <= 12; ++month)
		do_month_eval(month);
}

void do_hour_minute_eval(unsigned int hour, unsigned int minute)
{
	rtcc_timestamp stamp, new_stamp;
	char msg[100];

	setUp();

	sprintf(msg, "Testing hour-minute: %d:%d", hour, minute);

	stamp = rtcc_create_timestamp(&date);
	TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, stamp, "Date was not reset before method");

	date.hours = hour;
	date.minutes = minute;

	new_stamp = rtcc_create_timestamp(&date);

	stamp += ((uint32)hour)*3600ULL;
	stamp += ((uint32)minute)*60ULL;

	TEST_ASSERT_EQUAL_UINT32_MESSAGE(stamp, new_stamp, msg);
	TEST_ASSERT_EQUAL_UINT16(0, rtcc_timestamp_difference(stamp, new_stamp, NULL));
}

/*
 * Test that every day in the selected years starts with the correct timestamp, i.e. 12:00:00 am
 * is correct.
 */
void test_years()
{
	do_year_eval(0);
	do_year_eval(10);
	do_year_eval(15);
	do_year_eval(16);
}

void test_hours_minutes()
{
	do_hour_minute_eval(0, 0);
	do_hour_minute_eval(10, 52);
	do_hour_minute_eval(23, 15);
}

/*
 * test that each year starts with the correct timestamp for that year, i.e. jan 1, XXXX is correct
 * for all years supported by the RTCC (2000 - 2099)
 */
void test_year_starts()
{
	unsigned int 	year;
	rtcc_timestamp 	stamp;

	setUp();

	stamp = rtcc_create_timestamp(&date);

	for (year = 0; year <= 99; ++year)
	{
		char msg[100];

		sprintf(msg, "Year %d", 2000+year);

		rtcc_timestamp new_stamp;

		date.year = year;
		new_stamp = rtcc_create_timestamp(&date);

		TEST_ASSERT_EQUAL_UINT32_MESSAGE(stamp, new_stamp, msg);

		//Increment timestamp for the next year
		stamp += (24UL*60UL*60UL*365ULL);
		if ((year % 4) == 0)
			stamp += (24UL*60UL*60UL);
	}
}