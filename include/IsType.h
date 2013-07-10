#ifndef _ISTYPE_H_
#define _ISTYPE_H_

// Macro determining whether an object is of a certain type.
#define isType(objectPointer, Type) (dynamic_cast<Type*>(objectPointer) != nullptr)

#endif // _ISTYPE_H_
