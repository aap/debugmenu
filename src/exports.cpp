#include "debugmenu.h"


#define INTTYPES \
	X(Int8, int8) \
	X(Int16, int16) \
	X(Int32, int32) \
	X(Int64, int64) \
	X(UInt8, uint8) \
	X(UInt16, uint16) \
	X(UInt32, uint32) \
	X(UInt64, uint64)
#define FLOATTYPES \
	X(Float32, float) \
	X(Float64, double)

extern "C" {

#define X(NAME, TYPE) \
EXPORT void																			\
DebugMenuAdd##NAME(const char *path, const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound, const char **strings)	\
{																				\
	Menu *m = findMenu(path);																\
	if(m == nil)																		\
		return;																		\
	m->appendEntry(new MenuEntry_##NAME(name, ptr, triggerFunc, step, lowerBound, upperBound, strings));							\
}
INTTYPES
#undef X

#define X(NAME, TYPE) \
EXPORT void																			\
DebugMenuAdd##NAME(const char *path, const char *name, TYPE *ptr, TriggerFunc triggerFunc, TYPE step, TYPE lowerBound, TYPE upperBound)	\
{																				\
	Menu *m = findMenu(path);																\
	if(m == nil)																		\
		return;																		\
	m->appendEntry(new MenuEntry_##NAME(name, ptr, triggerFunc, step, lowerBound, upperBound));							\
}
FLOATTYPES
#undef X

EXPORT void																			\
DebugMenuAddCmd(const char *path, const char *name, TriggerFunc triggerFunc)	\
{																				\
	Menu *m = findMenu(path);																\
	if(m == nil)																		\
		return;																		\
	m->appendEntry(new MenuEntry_Cmd(name, triggerFunc));													\
}

}
