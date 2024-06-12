#include "memory.h"

#include <stdlib.h>

#include "compiler.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>

#include "debug.h"
#endif

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
    if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
        collectGarbage();
#endif
    }

    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void *result = realloc(pointer, newSize);
    if (result == NULL) exit(1);
    return result;
}

static void freeObject(Obj *object) {
#ifdef DEBUG_LOG_GC
    printf("%p free type %s\n", (void *)object, objTypeToString(object->type));
#endif

    switch (object->type) {
        case OBJ_CLOSURE: {
            ObjClosure *closure = (ObjClosure *)object;
            FREE_ARRAY(ObjUpvalue *, closure->upvalues, closure->upvalueCount);

            FREE(ObjClosure, object);
            break;
        }

        case OBJ_FUNCTION: {
            ObjFunction *function = (ObjFunction *)object;
            freeChunk((Chunk *)function->chunk);
            FREE(ObjFunction, object);
            break;
        }

        case OBJ_NATIVE: {
            FREE(ObjNative, object);
            break;
        }

        case OBJ_STRING: {
            ObjString *string = (ObjString *)object;
            reallocate(object, sizeof(ObjString) + string->length + 1, 0);
            break;
        }

        case OBJ_UPVALUE: {
            FREE(ObjUpvalue, object);
            break;
        }
    }
}

void freeObjects() {
    Obj *object = vm.objects;
    while (object != NULL) {
        Obj *next = (Obj *)object->next;
        freeObject(object);
        object = next;
    }
}

void markValue(Value value) {
    if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markRoots() {
    for (Value *slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj *)vm.frames[i].closure);
    }

    for (ObjUpvalue *upvalue = vm.openUpvalues;  //
         upvalue != NULL;                        //
         upvalue = (ObjUpvalue *)upvalue->next) {
        markObject((Obj *)upvalue);
    }

    markTable(&vm.globals);
    markCompilerRoots();
}

void collectGarbage() {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
#endif

    markRoots();

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
#endif
}
