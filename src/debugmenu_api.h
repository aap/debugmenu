
extern "C" {

struct MenuEntry;

typedef void (*TriggerFunc)(void);
EXPORT MenuEntry *DebugMenuAddInt8(const char *path, const char *name, int8 *ptr, TriggerFunc triggerFunc, int8 step, int8 lowerBound, int8 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddInt16(const char *path, const char *name, int16 *ptr, TriggerFunc triggerFunc, int16 step, int16 lowerBound, int16 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddInt32(const char *path, const char *name, int32 *ptr, TriggerFunc triggerFunc, int32 step, int32 lowerBound, int32 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddInt64(const char *path, const char *name, int64 *ptr, TriggerFunc triggerFunc, int64 step, int64 lowerBound, int64 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddUInt8(const char *path, const char *name, uint8 *ptr, TriggerFunc triggerFunc, uint8 step, uint8 lowerBound, uint8 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddUInt16(const char *path, const char *name, uint16 *ptr, TriggerFunc triggerFunc, uint16 step, uint16 lowerBound, uint16 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddUInt32(const char *path, const char *name, uint32 *ptr, TriggerFunc triggerFunc, uint32 step, uint32 lowerBound, uint32 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddUInt64(const char *path, const char *name, uint64 *ptr, TriggerFunc triggerFunc, uint64 step, uint64 lowerBound, uint64 upperBound, const char **strings);
EXPORT MenuEntry *DebugMenuAddFloat32(const char *path, const char *name, float *ptr, TriggerFunc triggerFunc, float step, float lowerBound, float upperBound);
EXPORT MenuEntry *DebugMenuAddFloat64(const char *path, const char *name, double *ptr, TriggerFunc triggerFunc, double step, double lowerBound, double upperBound);
EXPORT MenuEntry *DebugMenuAddCmd(const char *path, const char *name, TriggerFunc triggerFunc);
EXPORT void DebugMenuEntrySetWrap(MenuEntry *e, bool wrap);
EXPORT void DebugMenuEntrySetStrings(MenuEntry *e, const char **strings);

}