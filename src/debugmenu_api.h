
extern "C" {

typedef void (*TriggerFunc)(void);
EXPORT void DebugMenuAddInt8(const char *path, const char *name, int8 *ptr, TriggerFunc triggerFunc, int8 step, int8 lowerBound, int8 upperBound, const char **strings);
EXPORT void DebugMenuAddInt16(const char *path, const char *name, int16 *ptr, TriggerFunc triggerFunc, int16 step, int16 lowerBound, int16 upperBound, const char **strings);
EXPORT void DebugMenuAddInt32(const char *path, const char *name, int32 *ptr, TriggerFunc triggerFunc, int32 step, int32 lowerBound, int32 upperBound, const char **strings);
EXPORT void DebugMenuAddInt64(const char *path, const char *name, int64 *ptr, TriggerFunc triggerFunc, int64 step, int64 lowerBound, int64 upperBound, const char **strings);
EXPORT void DebugMenuAddUInt8(const char *path, const char *name, uint8 *ptr, TriggerFunc triggerFunc, uint8 step, uint8 lowerBound, uint8 upperBound, const char **strings);
EXPORT void DebugMenuAddUInt16(const char *path, const char *name, uint16 *ptr, TriggerFunc triggerFunc, uint16 step, uint16 lowerBound, uint16 upperBound, const char **strings);
EXPORT void DebugMenuAddUInt32(const char *path, const char *name, uint32 *ptr, TriggerFunc triggerFunc, uint32 step, uint32 lowerBound, uint32 upperBound, const char **strings);
EXPORT void DebugMenuAddUInt64(const char *path, const char *name, uint64 *ptr, TriggerFunc triggerFunc, uint64 step, uint64 lowerBound, uint64 upperBound, const char **strings);
EXPORT void DebugMenuAddFloat32(const char *path, const char *name, float *ptr, TriggerFunc triggerFunc, float step, float lowerBound, float upperBound);
EXPORT void DebugMenuAddFloat64(const char *path, const char *name, double *ptr, TriggerFunc triggerFunc, double step, double lowerBound, double upperBound);
EXPORT void DebugMenuAddCmd(const char *path, const char *name, TriggerFunc triggerFunc);

}