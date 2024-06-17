#ifndef clox_value_h
#define clox_value_h

#include "common.h"
#include "lines.h"

typedef enum { VAL_BOOL, VAL_NIL, VAL_NUMBER, VAL_OBJ } ValueType;

typedef enum {
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_STRING,
    OBJ_UPVALUE,
} ObjType;

typedef struct {
    ObjType type;
    bool isMarked;
    // forward struct declaration; pointer only.
    struct Obj *next;
} Obj;

typedef struct {
    ValueType type;
    union {
        bool boolean;
        double number;
        Obj *obj;
    } as;
} Value;

typedef struct {
    int capacity;
    int count;
    Value *values;
} ValueArray;

typedef struct {
    Obj obj;
    int length;
    uint32_t hash;
    char chars[];
} ObjString;

typedef struct {
    Obj obj;
    Value *location;
    Value closed;
    // forward struct declaration; pointer only.
    struct ObjUpvalue *next;
} ObjUpvalue;

typedef struct {
    Obj obj;
    int arity;
    // forward struct declaration; On stack; sync or [X_X] segfault.
    struct Chunk {
        int count;
        int capacity;
        uint8_t *code;
        Lines lines;
        ValueArray constants;
    } chunk;
    ObjString *name;
    int upvalueCount;
} ObjFunction;

typedef struct {
    Obj obj;
    ObjFunction *function;
    ObjUpvalue **upvalues;
    int upvalueCount;
} ObjClosure;

typedef Value (*NativeFn)(int argCount, Value *args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

typedef struct {
    Obj obj;
    ObjString *name;
} ObjClass;

typedef struct {
    Obj obj;
    ObjClass *klass;
    // forward struct declaration; On stack; sync or [X_X] segfault.
    struct Table {
        int count;
        int capacity;
        void *entries;
    } fields;
} ObjInstance;

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)
#define IS_CLASS(value) isObjType(value, OBJ_CLASS)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)
#define AS_CLASS(value) ((ObjClass *)AS_OBJ(value))
#define AS_CLOSURE(value) ((ObjClosure *)AS_OBJ(value))
#define AS_FUNCTION(value) ((ObjFunction *)AS_OBJ(value))
#define AS_INSTANCE(value) ((ObjInstance *)AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative *)AS_OBJ(value))->function)
#define AS_STRING(value) ((ObjString *)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString *)AS_OBJ(value))->chars)

#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(value) ((Value){VAL_OBJ, {.obj = (Obj *)value}})

bool valuesEqual(Value a, Value b);
void initValueArray(ValueArray *valueArray);
void freeValueArray(ValueArray *valueArray);
void writeValueArray(ValueArray *valueArray, Value value);

void printValue(Value value);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjClass *newClass(ObjString *name);
ObjClosure *newClosure(ObjFunction *function);
ObjFunction *newFunction();
ObjInstance *newInstance(ObjClass *klass);
ObjNative *newNative(NativeFn function);
ObjString *makeString(int length);
ObjString *takeString(const char *chars, int length, uint32_t hash);
ObjString *copyString(const char *chars, int length);
ObjUpvalue *newUpvalue(Value *slot);
uint32_t hashString(const char *key, int length);
void printObject(Value value);
#ifdef DEBUG_LOG_GC
void printDebugObjectHeader(const char *message, Obj *object);
#endif
const char *objTypeToString(ObjType type);
void markObject(Obj *object);

#endif