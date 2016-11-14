// prelude.h
#pragma once

/*
	 Common defines loaded as a prelude to all brando files
	 */

/*
	 functional helpers
	 */
#define exec(LAMBDA) ([]{ return LAMBDA })
