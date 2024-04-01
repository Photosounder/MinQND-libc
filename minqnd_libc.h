#ifndef MINQND_LIBC_H
#define MINQND_LIBC_H

//**** stdint.h ****

#include <stdint.h>


//**** limits.h & float.h ****

#define INT_MAX 0x7fffffff
#define FLT_MAX 3.4028235e38f
#define DBL_MAX 1.7976931348623147e+308


//**** math.h ****

#define NAN (0./0.)
#define INFINITY (1./0.)
#define M_PI 3.141592653589793

static inline uint32_t float_as_int(float f) { union {float f; uint32_t i;} u; u.f = f; return u.i; }
static inline uint64_t double_as_int(double f) { union {double f; uint64_t i;} u; u.f = f; return u.i; }
static inline double int_as_double(uint64_t i) { union {double f; uint64_t i;} u; u.i = i; return u.f; }
#define isnan(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_int(x) & 0x7fffffff) > 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_int(x) & -1ULL>>1) > 0x7ffULL<<52 : 0)
#define isfinite(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_int(x) & 0x7fffffff) < 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_int(x) & -1ULL>>1) < 0x7ffULL<<52 : 1)

extern double fmod(double x, double y);
extern double exp(double x);
extern double exp2(double x);
extern double log(double x);
extern double log2(double x);
extern double log10(double x);
extern double pow(double x, double y);
extern double cbrt(double x);
extern double sin(double x);
extern double cos(double x);
extern double sin_tr(double x);
extern double cos_tr(double x);
extern double tan(double x);
extern double atan2(double y, double x);
extern double asin(double x);
extern double hypot(double x, double y);
extern double tgamma(double x);
extern double erf(double x);

static float fabsf(float x) { return __builtin_fabsf(x); }
static double fabs(double x) { return __builtin_fabs(x); }
static float sqrtf(float x) { return __builtin_sqrtf(x); }
static double sqrt(double x) { return __builtin_sqrt(x); }
static float copysignf(float x, float y) { return __builtin_copysignf(x, y); }
static double copysign(double x, double y) { return __builtin_copysign(x, y); }
static float ceilf(float x) { return __builtin_ceilf(x); }
static double ceil(double x) { return __builtin_ceil(x); }
static float floorf(float x) { return __builtin_floorf(x); }
static double floor(double x) { return __builtin_floor(x); }
static float truncf(float x) { return __builtin_truncf(x); }
static double trunc(double x) { return __builtin_trunc(x); }
static float nearbyintf(float x) { return __builtin_nearbyintf(x); }
static double nearbyint(double x) { return __builtin_nearbyint(x); }
static float rintf(float x) { return __builtin_rintf(x); }
static double rint(double x) { return __builtin_rint(x); }
static double fma(double x, double y, double z) { return __builtin_fma(x, y, z); }


//**** ctype.h ****

extern int isspace(int c);
extern int isdigit(int c);
extern int isxdigit(int c);


//**** stdio.h ****

typedef int FILE;	// we don't actually use FILE for now
extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

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


//**** stdlib.h ****

extern int abs(int j);
extern long long int llabs(long long int j);
extern int atoi(const char *nptr);
extern double strtod(const char *nptr, char **endptr);
extern void srand(unsigned int seed);
extern int rand(void);
extern void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));


//**** time.h ****

typedef int64_t time_t;
struct tm { int tm_sec, tm_min, tm_hour, tm_mday, tm_mon, tm_year, tm_wday, tm_yday, tm_isdst; };
extern time_t time(time_t *timer);
extern double difftime(time_t time1, time_t time0);
extern struct tm *gmtime(const time_t *timer);
extern struct tm *gmtime_r(const time_t *timep, struct tm *result);
extern struct tm *localtime(const time_t *timer);
extern struct tm *localtime_r(const time_t *timep, struct tm *result);
extern time_t timegm(struct tm *tm);
extern time_t mktime(struct tm *timeptr);
extern size_t strftime(char * restrict s, size_t maxsize, const char *format, const struct tm *timeptr);


//**** Other ****

typedef long ssize_t;

#endif // MINQND_LIBC_H


#ifdef MINQND_LIBC_IMPLEMENTATION


//**** math.h ****

double fmod(double x, double y)
{
	return x;	// TODO
}

double exp(double x) { return exp2(x*1.4426950408889634); }
double exp2(double x)
{
	if (x < -1022.)	return 0.;
	if (x > 1024.)	return INFINITY;

	// Calculate the integer exponent part so that ye = 2^xe
	int64_t xe = floor(x);
	double ye = int_as_double((xe + 1023) << 52);

	// Fractional part, yf = 2^xf, error < 9e-16
	double xf = x - xe;
	double yf = (((((((((1.00091803e-08*xf + 9.43296e-08)*xf + 1.33185291e-06)*xf + 1.524399552e-05)*xf
			+ 0.00015404003239)*xf + 0.0013333541776)*xf + 0.009618129460644)*xf + 0.05550410862031)*xf
			+ 0.24022650696196)*xf + 0.6931471805598855)*xf + 0.9999999999999996;

	return ye * yf;
}

double log(double x) { return log2(x) * 0.6931471805599453; }
double log2(double x) { return x; }	// TODO
double log10(double x) { return log2(x) * 0.3010299956639812; }
double pow(double x, double y) { return exp2(log2(x) * y); }
double cbrt(double x) { return pow(x, 1./3.); }

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

double tan(double x)
{
	return x;	// TODO
}

double atan2(double y, double x)	// error < 1.05e-15 radians
{
	double xa = fabs(x), ya = fabs(y);
	double z = (ya-xa) / (ya+xa);
	double z2 = z * z;
	z = (((((((((((((((((-6.10762823e-06*z2 + 6.31378902921e-05)*z2 - 0.000308754446813)*z2 + 0.0009572739170519)*z2 - 0.00213695014982195)*z2 + 0.00372040345104847)*z2 - 0.00538302610598414)*z2 + 0.006857690908378)*z2 - 0.008107837710245)*z2 + 0.009283843417554)*z2 - 0.010592893430548)*z2 + 0.012239782366304361)*z2 - 0.0144682809894434)*z2 + 0.0176838534251776)*z2 - 0.022736418880277)*z2 + 0.0318309885702715)*z2 - 0.053051647696608)*z2 + 0.15915494309189251)*z + 0.125;
	if (x < 0.)	z = 0.5 - z;
	if (y < 0.)	z = -z;
	z = z * 2.*M_PI;
	return z;
}

double asin(double x)
{
	return x;	// TODO
}

double hypot(double x, double y)
{
	return sqrt(x*x + y*y);
}

double tgamma(double x) { return NAN; }	// TODO

double erf(double x)
{
	double y, xa = fabs(x);

	if (xa > 6.)
		y = 1.;
	else
	{
		// erf(x) ~= 1 - polynomial^-8 for x >= 0, max error 1.5e-14
		y = ((((((((((((((((((-6.667956e-13*xa + 2.14101634e-11)*xa - 3.084902216e-10)*xa + 2.6540443454e-09)*xa - 1.489560518e-08)*xa + 5.49541754e-08)*xa - 1.1233622663e-07)*xa - 3.293126682e-08)*xa + 1.1916855675e-06)*xa - 3.9857655388e-06)*xa + 7.156348982e-06)*xa + 1.59932457175e-05)*xa - 2.8627191393e-05)*xa + 0.00034373554487)*xa + 0.001270979388688)*xa + 0.00339526696247)*xa + 0.02453844565104)*xa + 0.089524655472295)*xa + 0.14104739588732)*xa + 1-1e-15;

		// y = 1 - y^-8
		y = y*y;
		y = y*y;
		y = y*y;
		y = 1. - 1./y;
	}
	return x < 0. ? -y : y;
}


//**** ctype.h ****

int isspace(int c) { return c == ' ' || (unsigned)c-'\t' < 5; }
int isdigit(int c) { return (unsigned)c-'0' < 10; }
int isxdigit(int c) { return isdigit(c) || ((unsigned)c|32)-'a' < 6; }


//**** stdio.h ****

FILE *const stdin = NULL;
FILE *const stdout = NULL;
FILE *const stderr = NULL;

//**** stdlib.h ****

int abs(int j) { return j > 0 ? j : -j; }
long long int llabs(long long int j) { return j > 0 ? j : -j; }
int atoi(const char *nptr) { int v=0; sscanf(nptr, "%d", &v); return v; }

double strtod(const char *nptr, char **endptr)
{
	double v = NAN;
	int n = 0;
	sscanf(nptr, "%lg%n", &v, &n);
	*endptr = (char *) &nptr[n];
	return v;
}

static uint64_t libc_rand_seed;
void srand(unsigned int seed)
{
	libc_rand_seed = seed - 1;
}

int rand(void)
{
	libc_rand_seed = 6364136223846793005ULL * libc_rand_seed + 1;
	return libc_rand_seed >> 33;
}

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{	// Ray Gardner's 1990 public domain shell sort
	size_t wnel, gap, wgap, i, j, k;
	char *a, *b, tmp;

	wnel = size * nmemb;
	for (gap = 0; ++gap < nmemb;)
		gap *= 3;
	while ((gap /= 3) != 0)
	{
		wgap = size * gap;
		for (i = wgap; i < wnel; i += size)
			for (j = i - wgap; ;j -= wgap)
			{
				a = j + (char *)base;
				b = a + wgap;
				if ((*compar)(a, b) <= 0)
					break;
				k = size;
				do {
					tmp = *a;
					*a++ = *b;
					*b++ = tmp;
				}
				while (--k);
				if (j < wgap)
					break;
			}
	}
}

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


//**** time.h ****

time_t time(time_t *timer) { return (time_t) -1; }				// TODO
double difftime(time_t time1, time_t time0) { return time1 - time0; }
struct tm *gmtime(const time_t *timer) { return NULL; }				// TODO
struct tm *gmtime_r(const time_t *timep, struct tm *result) { return NULL; }	// TODO
struct tm *localtime(const time_t *timer) { return NULL; }			// TODO
struct tm *localtime_r(const time_t *timep, struct tm *result) { return NULL; }	// TODO
time_t timegm(struct tm *tm) { return (time_t) -1; }				// TODO
time_t mktime(struct tm *timeptr) { return (time_t) -1; }			// TODO
size_t strftime(char * restrict s, size_t maxsize, const char *format, const struct tm *timeptr) { return 0; }	// TODO


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
