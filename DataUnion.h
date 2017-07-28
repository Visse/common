#pragma once

#include "PPUtils.h"

#include <type_traits>



#define _DATA_UNION_COSTRUCTOR_HELPER1( Type, Name ) Type data
#define _DATA_UNION_CONSTRUCTOR_HELPER2( Type, Name ) Name(data)

#define _DATA_UNION_CONSTRUCTOR( Name, Args )         \
    Name(_DATA_UNION_COSTRUCTOR_HELPER1 Args) : \
        _DATA_UNION_CONSTRUCTOR_HELPER2 Args {}

#define _DATA_UNION_MEMBERS_HELPER( Type, Name )  Type Name;


#define _DATA_UNION_MEMBERS(Name, Args) \
    _DATA_UNION_MEMBERS_HELPER Args

#ifdef _MSC_VER
// Disable warning C4582  - constructor is not implicitly called 
// Disable warning C5027  - move assignment operator was implicitly defined as deleted
#define _DATA_UNION_PRE                 \
    __pragma(warning (suppress: 4582 5027))

#define _DATA_UNION_POST

#else

#define _DATA_UNION_PRE
#define _DATA_UNION_POST

#endif

/**  CREATE_DATA_UNION - Creates a data union with the specifed name and members
  * Usage: 
  *     CREATE_DATA_UNION( DataUnion, DataType, (Type1, name1), (Type2, name2), ..etc )
  *  Result:
  *     union DataUnion {
  *         DataType type;
  *         Type1 name1;
  *         Type2 name2;
  *         DataUnion( Type1 data ) : name1(data) {}
  *         DataUnion( Type2 data ) : name2(data) {}
  *    }
  */
#define CREATE_DATA_UNION( Name, TypeEnum, ... )                        \
    union Name {                                                        \
        _DATA_UNION_PRE                                                 \
        TypeEnum type;                                                  \
        PP_UTILS_MAP(Name, _DATA_UNION_MEMBERS, __VA_ARGS__)            \
        PP_UTILS_MAP(Name, _DATA_UNION_CONSTRUCTOR, __VA_ARGS__)        \
        _DATA_UNION_POST                                                \
    }



#define _CREATE_DATA_STRUCT_MEMBERS_1( Type, Name ) Type Name;
#define _CREATE_DATA_STRUCT_MEMBERS( Name, Args ) _CREATE_DATA_STRUCT_MEMBERS_1 Args
#define _CREATE_DATA_STRUCT_FUNC_ARGS1( Type, Name ) Type Name,
#define _CREATE_DATA_STRUCT_FUNC_ARGS( Name, Args ) _CREATE_DATA_STRUCT_FUNC_ARGS1 Args
#define _CREATE_DATA_STRUCT_ASSIGN1( Type, Name ) _tmp.Name = Name;
#define _CREATE_DATA_STRUCT_ASSIGN( Name, Args ) _CREATE_DATA_STRUCT_ASSIGN1 Args

#define CREATE_DATA_STRUCT( Name, TypeEnum, ...)    \
        struct Name##Data {                         \
            TypeEnum type = TypeEnum::Name;         \
            PP_UTILS_MAP(Name, _CREATE_DATA_STRUCT_MEMBERS, __VA_ARGS__) \
        };                                          \
        inline Name##Data Name( PP_UTILS_MAP(Name, _CREATE_DATA_STRUCT_FUNC_ARGS, __VA_ARGS__) int _=0 ) { \
            Name##Data _tmp;                        \
            PP_UTILS_MAP(Name, _CREATE_DATA_STRUCT_ASSIGN, __VA_ARGS__) \
            return _tmp;                            \
        }
