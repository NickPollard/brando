// brando.h
//
// include this file in your project and #define BRANDO_MAIN to pull in required single-compilation code

#ifdef BRANDO_MAIN
#include "unit.h"
#include "concurrent/time.h"

namespace brando {
	Unit unit() { return Unit(); }

  namespace concurrent {
		Seconds seconds;
		Minutes minutes;
		Hours hours;
  }
}

#endif
