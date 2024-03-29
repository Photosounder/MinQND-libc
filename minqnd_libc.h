#ifndef MINQND_LIBC_H
#define MINQND_LIBC_H

//**** stdint.h ****
#include <stdint.h>


//**** math.h ****

#define NAN (0./0.)
#define INFINITY (1./0.)
#define M_PI 3.141592653589793

static inline uint32_t float_as_u32(float f) { union {float f; uint32_t i;} u; u.f = f; return u.i; }
static inline uint64_t double_as_u64(double f) { union {double f; uint64_t i;} u; u.f = f; return u.i; }
static inline double u64_as_double(uint64_t i) { union {double f; uint64_t i;} u; u.i = i; return u.f; }
#define isnan(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_u32(x) & 0x7fffffff) > 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_u64(x) & -1ULL>>1) > 0x7ffULL<<52 : 0)
#define isfinite(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_u32(x) & 0x7fffffff) < 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_u64(x) & -1ULL>>1) < 0x7ffULL<<52 : 1)
#define abs(x) ((x) >= 0 ? x : -x)

extern double exp(double x);
extern double exp2(double x);
extern double sin(double x);
extern double cos(double x);
extern double sin_tr(double x);
extern double cos_tr(double x);

#define fabsf __builtin_fabsf
#define fabs __builtin_fabs
#define sqrtf __builtin_sqrtf
#define sqrt __builtin_sqrt
#define copysignf __builtin_copysignf
#define copysign __builtin_copysign
#define ceilf __builtin_ceilf
#define ceil __builtin_ceil
#define floorf __builtin_floorf
#define floor __builtin_floor
#define truncf __builtin_truncf
#define trunc __builtin_trunc
#define nearbyintf __builtin_nearbyintf
#define nearbyint __builtin_nearbyint
#define rintf __builtin_rintf
#define rint __builtin_rint


//**** ctype.h ****

extern int isspace(int c);
extern int isdigit(int c);
extern int isxdigit(int c);


//**** stdio.h ****

#include <stddef.h>	// for NULL
#define size_t unsigned long	// correct on wasm32 and wasm64
#define EOF (-1)

#define va_list __builtin_va_list
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

extern void *memset(void *s, int c, size_t n);
extern void *memcpy(void *s1, const void *s2, size_t n);
extern void *memmove(void *s1, const void *s2, size_t n);
extern size_t strlen(const char *s);
extern char *strchr(const char *s, int c);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern char *strstr(const char *s1, const char *s2);
extern char *strncpy(char *s1, const char *s2, size_t n);
extern int vsnprintf(char *s, size_t n, const char *format, va_list arg);
extern int vsprintf(char *s, const char *format, va_list arg);
extern int snprintf(char *s, size_t n, const char *format, ...);
extern int sprintf(char *s, const char *format, ...);
extern int vsscanf(const char *s, const char *format, va_list arg);
extern int sscanf(const char *s, const char *format, ...);

#endif // MINQND_LIBC_H


#ifdef MINQND_LIBC_IMPLEMENTATION

//**** math.h ****

double exp(double x) { return exp2(x*1.4426950408889634); }
double exp2(double x)
{
	if (x < -1022.)	return 0.;
	if (x > 1024.)	return INFINITY;

	// Calculate the integer exponent part so that ye = 2^xe
	int64_t xe = floor(x);
	double ye = u64_as_double((xe + 1023) << 52);

	// Fractional part, yf = 2^xf, error < 9e-16
	double xf = x - xe;
	double yf = (((((((((1.00091803e-08*xf + 9.43296e-08)*xf + 1.33185291e-06)*xf + 1.524399552e-05)*xf
			+ 0.00015404003239)*xf + 0.0013333541776)*xf + 0.009618129460644)*xf + 0.05550410862031)*xf
			+ 0.24022650696196)*xf + 0.6931471805598855)*xf + 0.9999999999999996;

	return ye * yf;
}

double sin(double x) { return sin_tr(x * (1./(2.*M_PI))); }
double cos(double x) { return cos_tr(x * (1./(2.*M_PI))); }
double sin_tr(double x) { return cos_tr(x - 0.25); }
double cos_tr(double x)
{
	// x --> [-0.25 , 0.25]
	x = fabs(x - floor(x) - 0.5) - 0.25;

	// Error < 1e-15
	double x2 = x * x;
	return (((((((-0.694404*x2 + 3.817861)*x2 - 15.094558732)*x2 + 42.058692851)*x2 - 76.7058597769)*x2
			+ 81.605249276959313)*x2 - 41.341702240408)*x2 + 6.2831853071796)*x;
}


//**** ctype.h ****

int isspace(int c) { return c == ' ' || (unsigned)c-'\t' < 5; }
int isdigit(int c) { return (unsigned)c-'0' < 10; }
int isxdigit(int c) { return isdigit(c) || ((unsigned)c|32)-'a' < 6; }


//**** string.h ****

void *memset(void *dest, int c, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD)
		return __builtin_memset(dest, c, n);
#endif
	char *s = dest;
	for (; n; n--, s++)
		*s = c;
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD)
		return __builtin_memcpy(dest, src, n);
#endif
	char *d = dest;
	const char *s = src;
	for (; n; n--)
		*d++ = *s++;
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD)
		return __builtin_memmove(dest, src, n);
#endif
	char *d = dest;
	const char *s = src;

	if (d == s) return d;
	if ((uintptr_t)s-(uintptr_t)d-n <= -2*n)
		return memcpy(d, s, n);

	if (d < s)
		for (; n; n--)
			*d++ = *s++;
	else
		while (n) n--, d[n] = s[n];

	return dest;
}

size_t strlen(const char *s)
{
	const char *e;
	for (e=s; *e; e++) {}
	return e-s;
}

char *strchr(const char *s, int c)
{
	c = (unsigned char)c;
	if (!c) return (char *)s + strlen(s);
	for (; *s && *(unsigned char *)s != c; s++);
	return *(unsigned char *)s == (unsigned char)c ? (char *)s : 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *l=(void *)s1, *r=(void *)s2;
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}

char *strstr(const char *s1, const char *s2) 
{
	char c, sc;
	size_t len;

	if ((c = *s2++) != 0)
	{
		len = strlen(s2);
		do
		{
			do
			{
				if ((sc = *s1++) == 0)
					return NULL;
			}
			while (sc != c);
		}
		while (strncmp(s1, s2, len) != 0);
		s1--;
	}
	return (char *) s1;
}

char *strncpy(char *s1, const char *s2, size_t n)
{
	for (; n && (*s1=*s2); n--, s2++, s1++) {}
	memset(s1, 0, n);
	return s1;
}

#include "minqnd_sprintf.c"
#include "minqnd_sscanf.c"


//**** crt1-reactor.c ****

#if defined(_REENTRANT)
#include <stdatomic.h>
extern void __wasi_init_tp(void);
#endif
extern void __wasm_call_ctors(void);

__attribute__((export_name("_initialize"))) void _initialize(void)
{
#if defined(_REENTRANT)
	static volatile atomic_int initialized = 0;
	int expected = 0;
	if (!atomic_compare_exchange_strong(&initialized, &expected, 1))
		__builtin_trap();
	__wasi_init_tp();
#else
	static volatile int initialized = 0;
	if (initialized != 0)
		__builtin_trap();
	initialized = 1;
#endif
	__wasm_call_ctors();
}

#endif // MINQND_LIBC_IMPLEMENTATION
