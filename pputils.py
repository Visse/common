import sys

if len(sys.argv) != 3:
    print("Usage: %s COUNT OUTPUT"%sys.argv[0])
    sys.exit(-1)


count = int(sys.argv[1])

file = open(sys.argv[2], 'w')

print('''#pragma once
// === WARNING - DO NOT EDIT - AUTO GENERATED FILE ===
// This file was generated by %s
// Full command line: %s

// Workaround for visual studio that has a bad implentation of __VA_ARGS__ :/ (it expands to a single argument if you pass it to another macro)
#define PP_UTILS_EXPAND_VARGS( FUNC, ARGS ) FUNC ARGS
#define _PP_UTILS_JOIN4( A, B ) A ## B
#define _PP_UTILS_JOIN3( A, B ) _PP_UTILS_JOIN4(A,B)
#define _PP_UTILS_JOIN2( A, B ) _PP_UTILS_JOIN3(A,B)
#define _PP_UTILS_JOIN( A, B ) _PP_UTILS_JOIN2(A,B)
'''%(sys.argv[0]," ".join(sys.argv[:])), file=file)

print('#define _PP_UTILS_NARGS_HELPER( %s, N, ... ) N'%', '.join([ '_'+str(i) for i in range(1,count+1)]), file=file)
print('#define PP_UTILS_NARGS( ... ) PP_UTILS_EXPAND_VARGS(_PP_UTILS_NARGS_HELPER,(,##__VA_ARGS__, %s)) '%', '.join([str(i) for i in reversed(range(0,count))]), file=file)

print('', file=file)

print('''
#define _PP_UTILS_UNWRAP_HELPER(...) __VA_ARGS__
#define PP_UTILS_UNWRAP( ARGS ) PP_UTILS_EXPAND_VARGS(_PP_UTILS_UNWRAP_HELPER, ARGS)
''', file=file)

print('', file=file)

print('#define _PP_UTILS_MAP_HELPER_0(NAME, FUNC)', file=file)
for i in range(1,count+1):
    argList = ', '.join(['_%i'%i for i in range(1,i+1)])
    funcList = ' '.join(['FUNC(NAME, _%i)'%i for i in range(1,i+1)])
    print('''#define _PP_UTILS_MAP_HELPER_%i(NAME, FUNC, %s) \\
        %s'''%(i, argList, funcList), file=file)



print('''

#define PP_UTILS_MAP( NAME, FUNC, ... ) PP_UTILS_EXPAND_VARGS(_PP_UTILS_JOIN(_PP_UTILS_MAP_HELPER_, PP_UTILS_EXPAND_VARGS(PP_UTILS_NARGS, (__VA_ARGS__))), (NAME, FUNC, __VA_ARGS__))

''', file=file)
