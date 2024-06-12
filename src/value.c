
#include "value.h"

#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "vm.h"

void initValueArray(ValueArray* valueArray) {
    valueArray->count = 0;
    valueArray->capacity = 0;
    valueArray->values = NULL;
}

void writeValueArray(ValueArray* valueArray, Value value) {
    if (valueArray->capacity < valueArray->count + 1) {
        int oldCapacity = valueArray->capacity;
        valueArray->capacity = GROW_CAPACITY(oldCapacity);
        valueArray->values = GROW_ARRAY(Value, valueArray->values, oldCapacity,
                                        valueArray->capacity);
    }

    valueArray->values[valueArray->count++] = value;
}

void freeValueArray(ValueArray* valueArray) {
    FREE_ARRAY(Value, valueArray->values, valueArray->capacity);
    initValueArray(valueArray);
}

const char* objTypeToString(ObjType type) {
    static char buffer[256];

    switch (type) {
        case OBJ_FUNCTION:
            return "OBJ_FUNCTION";
        case OBJ_NATIVE:
            return "OBJ_NATIVE";
        case OBJ_STRING:
            return "OBJ_STRING";
        case OBJ_UPVALUE:
            return "OBJ_UPVALUE";
        case OBJ_CLOSURE:
            return "OBJ_CLOSURE";
        default:
            sprintf(buffer, "unknown %d", type);
            return buffer;
    }
}

static void printFunction(ObjFunction* function) {
    if (function->name == NULL) {
        printf("<script>");
        return;
    }

    printf("<fn %s>", function->name->chars);
}

void printObject(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_CLOSURE:
            printFunction(AS_CLOSURE(value)->function);
            break;

        case OBJ_FUNCTION:
            printFunction(AS_FUNCTION(value));
            break;

        case OBJ_NATIVE:
            printf("<native fn>");
            break;

        case OBJ_STRING:
            printf("%s", AS_CSTRING(value));
            break;

        case OBJ_UPVALUE:
            printf("upvalue");
            break;
    }
}

void printValue(Value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;
        case VAL_OBJ:
            printObject(value);
    }
}

bool valuesEqual(Value a, Value b) {
    if (a.type != b.type) return false;

    switch (a.type) {
        case VAL_BOOL:
            return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:
            return true;
        case VAL_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:
            return AS_OBJ(a) == AS_OBJ(b);
        default:
            // Unreachable.
            printf("Fatal: unreachable == operator type %d\n", a.type);
            exit(1);
    }
}

static Obj* allocateObject(size_t size, ObjType type) {
    Obj* object = (Obj*)reallocate(NULL, 0, size);
    object->type = type;
    object->isMarked = false;

    object->next = (struct Obj*)vm.objects;
    vm.objects = object;
#ifdef DEBUG_LOG_GC
    printf("%p allocate %zu for %s\n", (void*)object, size,
           objTypeToString(type));
#endif
    return object;
}

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

ObjClosure* newClosure(ObjFunction* function) {
    ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
    for (int i = 0; i < function->upvalueCount; i++) {
        upvalues[i] = NULL;
    }

    ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalueCount = function->upvalueCount;
    return closure;
}

ObjFunction* newFunction() {
    ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
    Chunk* chunk = NULL;
    chunk = (Chunk*)realloc(chunk, sizeof(Chunk));
    if (chunk == NULL) {
        printf("vm: not enough memory for newFunction chunk\n");
        exit(1);
    }

    function->chunk = (void*)chunk;
    function->arity = 0;
    function->upvalueCount = 0;
    function->name = NULL;
    initChunk((Chunk*)function->chunk);
    return function;
}

ObjNative* newNative(NativeFn function) {
    ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
    native->function = function;
    return native;
}

uint32_t hashString(const char* key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

ObjString* makeString(int length) {
    ObjString* string =
        (ObjString*)allocateObject(sizeof(ObjString) + length + 1, OBJ_STRING);
    string->length = length;
    return string;
}

ObjString* takeString(const char* chars, int length, uint32_t hash) {
    ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
    if (interned != NULL) return interned;

    ObjString* string = makeString(length);
    memcpy(string->chars, chars, length);
    string->chars[length] = '\0';
    string->hash = hash;
    tableSet(&vm.strings, string, NIL_VAL);

    return string;
}

ObjString* copyString(const char* chars, int length) {
    uint32_t hash = hashString(chars, length);
    return takeString(chars, length, hash);
}

ObjUpvalue* newUpvalue(Value* slot) {
    ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
    upvalue->location = slot;
    upvalue->next = NULL;
    upvalue->closed = NIL_VAL;
    return upvalue;
}

void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

#ifdef DEBUG_LOG_GC
    printf("%p mark [%s] ", (void*)object, objTypeToString(object->type));
    printValue(OBJ_VAL(object));
    printf("\n");
#endif
    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack =
            (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);

        if (vm.grayStack == NULL) {
            printf("vm: not enough memory for grayStack\n");
            exit(1);
        }
    }
    vm.grayStack[vm.grayCount++] = object;
}