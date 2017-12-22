#pragma once


#include "PPUtils.h"


// Helpers for creating const-operators (+ - / * etc..)
#define _CREATE_DEFAULT_OPERATOR_CONST_HELPER2(OP, MEMBER) \
    copy.MEMBER OP other.MEMBER;
    
#define _CREATE_DEFAULT_OPERATOR_CONST_HELPER(OP, ...) \
    PP_UTILS_MAP(OP, _CREATE_DEFAULT_OPERATOR_CONST_HELPER2, __VA_ARGS__ )

#define _CREATE_DEFAULT_OPERATOR_CONST(TYPE, OP, MEM_OP, MEMBERS)               \
    TYPE operator OP ( const TYPE &other ) const                                \
    {                                                                           \
        TYPE copy(*this);                                                       \
        _CREATE_DEFAULT_OPERATOR_CONST_HELPER(MEM_OP, PP_UTILS_UNWRAP(MEMBERS)) \
        return copy;                                                            \
    }

// Define macros for creating const operators
#define CREATE_OP_ADD(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR_CONST(TYPE, +, +=, MEMBERS)

#define CREATE_OP_SUB(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR_CONST(TYPE, -, -=, MEMBERS)

#define CREATE_OP_MUL(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR_CONST(TYPE, *, *=, MEMBERS)

#define CREATE_OP_DIV(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR_CONST(TYPE, /, /=, MEMBERS)



// Helpers for creating nonconst-operators (+= -= /= *= etc..)
#define _CREATE_DEFAULT_OPERATOR_HELPER2(OP, MEMBER) \
    this->MEMBER OP other.MEMBER;
    
#define _CREATE_DEFAULT_OPERATOR_HELPER(OP, ...) \
    PP_UTILS_MAP(OP, _CREATE_DEFAULT_OPERATOR_HELPER2, __VA_ARGS__ )

#define _CREATE_DEFAULT_OPERATOR(TYPE, OP, MEMBERS)                     \
    TYPE& operator OP ( const TYPE &other )                             \
    {                                                                   \
        _CREATE_DEFAULT_OPERATOR_HELPER(OP, PP_UTILS_UNWRAP(MEMBERS))   \
        return *this;                                                   \
    }
    
#define CREATE_OP_ADD_ASSIGN(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR(TYPE, +=, MEMBERS)

#define CREATE_OP_SUB_ASSIGN(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR(TYPE, -=, MEMBERS)

#define CREATE_OP_MUL_ASSIGN(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR(TYPE, *=, MEMBERS)

#define CREATE_OP_DIV_ASSIGN(TYPE, MEMBERS) \
    _CREATE_DEFAULT_OPERATOR(TYPE, /=, MEMBERS)


// Helper macro for creating multiple operators at once


#define _CREATE_DEFAULT_OPERATORS_TYPE(TYPE, MEMBERS) TYPE
#define _CREATE_DEFAULT_OPERATORS_MEMBERS(TYPE, MEMBERS) MEMBERS

#define _CREATE_DEFAULT_OPERATORS_HELPER4(TYPE_MEMBERS, OPERATOR) \
    OPERATOR(PP_UTILS_EXPAND_VARGS(_CREATE_DEFAULT_OPERATORS_TYPE, TYPE_MEMBERS), PP_UTILS_EXPAND_VARGS(_CREATE_DEFAULT_OPERATORS_MEMBERS, TYPE_MEMBERS))

#define _CREATE_DEFAULT_OPERATORS_HELPER3(TYPE, MEMBERS, ...) PP_UTILS_MAP((TYPE,MEMBERS), _CREATE_DEFAULT_OPERATORS_HELPER4, __VA_ARGS__)
#define _CREATE_DEFAULT_OPERATORS_HELPER2(...)  PP_UTILS_EXPAND_VARGS(_CREATE_DEFAULT_OPERATORS_HELPER3, (__VA_ARGS__))

#define _CREATE_DEFAULT_OPERATORS_HELPER(...) _CREATE_DEFAULT_OPERATORS_HELPER2(__VA_ARGS__)

#define CREATE_DEFAULT_OPERATORS(TYPE, OPERATORS, MEMBERS) \
   PP_UTILS_EXPAND_VARGS(_CREATE_DEFAULT_OPERATORS_HELPER, (TYPE, MEMBERS, PP_UTILS_UNWRAP(OPERATORS)))

