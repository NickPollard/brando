// prelude.h
#pragma once

/*
	 Common defines loaded as a prelude to all brando files
	 */

/*
	 Handy syntax alises for more readable code
	 */
//#define type typename
#define var auto
#define val const auto

/*
	 functional helpers
	 */
#define exec(LAMBDA) ([]{ return LAMBDA })
