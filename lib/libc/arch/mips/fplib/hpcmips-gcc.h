
/*
-------------------------------------------------------------------------------
One of the macros `BIGENDIAN' or `LITTLEENDIAN' must be defined.
-------------------------------------------------------------------------------
*/
#define LITTLEENDIAN

/*
-------------------------------------------------------------------------------
The macro `BITS64' can be defined to indicate that 64-bit integer types are
supported by the compiler.
-------------------------------------------------------------------------------
*/
#define BITS64

/*
-------------------------------------------------------------------------------
Each of the following `typedef's defines the most convenient type that holds
integers of at least as many bits as specified.  For example, `uint8' should
be the most convenient type that can hold unsigned integers of as many as
8 bits.  The `flag' type must be able to hold either a 0 or 1.  For most
implementations of C, `flag', `uint8', and `int8' should all be `typedef'ed
to the same as `int'.
-------------------------------------------------------------------------------
*/
typedef int flag;
typedef unsigned char uint8;
typedef signed char int8;
typedef int uint16;
typedef int int16;
typedef unsigned int uint32;
typedef signed int int32;
#ifdef BITS64
typedef unsigned long long int uint64;
typedef signed long long int int64;
#endif

/*
-------------------------------------------------------------------------------
Each of the following `typedef's defines a type that holds integers
of _exactly_ the number of bits specified.  For instance, for most
implementation of C, `bits16' and `sbits16' should be `typedef'ed to
`unsigned short int' and `signed short int' (or `short int'), respectively.
-------------------------------------------------------------------------------
*/
typedef unsigned char bits8;
typedef signed char sbits8;
typedef unsigned short int bits16;
typedef signed short int sbits16;
typedef unsigned int bits32;
typedef signed int sbits32;
#ifdef BITS64
typedef unsigned long long int bits64;
typedef signed long long int sbits64;
#endif

#ifdef BITS64
/*
-------------------------------------------------------------------------------
The `LIT64' macro takes as its argument a textual integer literal and
if necessary ``marks'' the literal as having a 64-bit integer type.
For example, the GNU C Compiler (`gcc') requires that 64-bit literals be
appended with the letters `LL' standing for `long long', which is `gcc's
name for the 64-bit integer type.  Some compilers may allow `LIT64' to be
defined as the identity macro:  `#define LIT64( a ) a'.
-------------------------------------------------------------------------------
*/
#define LIT64( a ) a##LL
#endif

/*
-------------------------------------------------------------------------------
The macro `INLINE' can be used before functions that should be inlined.  If
a compiler does not support explicit inlining, this macro should be defined
to be `static'.
-------------------------------------------------------------------------------
*/
#define INLINE extern inline

#define float32_add			__addsf3
#define float64_add			__adddf3
#define float32_sub			__subsf3
#define float64_sub			__subdf3
#define float32_mul			__mulsf3
#define float64_mul			__muldf3
#define float32_div			__divsf3
#define float64_div			__divdf3
#define int32_to_float32		__floatsisf
#define int32_to_float64		__floatsidf
#define float32_to_int32_round_to_zero	__fixsfsi
#define float64_to_int32_round_to_zero	__fixdfsi
#define float32_to_uint32_round_to_zero	__fixunssfsi
#define float64_to_uint32_round_to_zero	__fixunsdfsi
#define float32_to_float64		__extendsfdf2
#define float64_to_float32		__truncdfsf2
