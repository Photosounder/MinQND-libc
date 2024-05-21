#ifndef MINQND_LIBC_H
#define MINQND_LIBC_H

//**** stdint.h ****

#include <stdint.h>


//**** limits.h & float.h ****

#define INT_MAX 0x7fffffff
#define FLT_MAX 3.4028235e38f
#define DBL_MAX 1.7976931348623147e+308
#define PATH_MAX 4096


//**** inttypes.h ****

#if UINTPTR_MAX == UINT64_MAX
  #define __PRI64  "l"
  #define __PRIPTR "l"
#else
  #define __PRI64  "ll"
  #define __PRIPTR ""
#endif
#define PRIdMAX __PRI64 "d"


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
extern float expf(float x);
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
extern double atan(double x);
extern double atan2(double y, double x);
extern float atan2f(float y, float x);
extern double asin(double x);
extern float asinf(float x);
extern double acos(double x);
extern double sinh(double x);
extern double cosh(double x);
extern double tanh(double x);
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
static long lroundf(float x) { return __builtin_nearbyintf(x); }
static double fma(double x, double y, double z)
{
#if __has_builtin(__builtin_wasm_relaxed_madd_f64x2)
	__attribute__((__vector_size__(2 * sizeof(double)))) double r2, x2 = {x}, y2 = {y}, z2 = {z};
	r2 = __builtin_wasm_relaxed_madd_f64x2(x2, y2, z2);
	return r2[0];
#else
	return x*y + z;
#endif
}


//**** ctype.h ****

extern int isspace(int c);
extern int isdigit(int c);
extern int isxdigit(int c);
extern int isalpha(int c);
extern int isalnum(int c);
extern int isupper(int c);
extern int tolower(int c);


//**** stdio.h ****

#include <stddef.h>	// for NULL
#define size_t unsigned long	// correct on wasm32 and wasm64
#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define va_list __builtin_va_list
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

typedef int FILE;	// we don't actually use FILE for now
extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;
static char *fgets(char *s, int n, FILE *stream) { return NULL; }
static int fprintf(FILE *stream, const char *format, ...) { return -1; }
static FILE *fopen(const char *filename, const char *mode) { return NULL; }
static int fclose(FILE *stream) { return EOF; }
static size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) { return EOF; }
static size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) { return EOF; }
static int fseek(FILE *stream, long int offset, int whence) { return EOF; }
static long int ftell(FILE *stream) { return 0; }
static void rewind(FILE *stream) { }

extern void *memset(void *s, int c, size_t n);
extern void *memcpy(void *s1, const void *s2, size_t n);
extern void *memmove(void *s1, const void *s2, size_t n);
extern size_t strlen(const char *s);
extern void *memchr(const void *s, int c, size_t n);
extern void *memrchr(const void *s, int c, size_t n);
extern char *strchr(const char *s, int c);
extern char *strrchr(const char *s, int c);
extern char *strpbrk(const char *s1, const char *s2);
extern char *strstr(const char *s1, const char *s2);
extern int strcmp(const char *s1, const char *s2);
extern int strncmp(const char *s1, const char *s2, size_t n);
extern int memcmp(const void *s1, const void *s2, size_t n);
extern char *strcpy(char *s1, const char *s2);
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
extern double atof(const char *nptr);
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

double fmod(double x, double y) { return x - trunc(x / y) * y; }

double exp(double x) { return exp2(x*1.4426950408889634); }
float expf(float x) { return exp(x); }
double exp2(double x)
{
	if (x < -1022.)	return 0.;
	if (x > 1024.)	return INFINITY;

	// Calculate the integer exponent part so that ye = 2^xe
	int64_t xe = floor(x);
	double ye = int_as_double((xe + 1023) << 52);

	// Fractional part, yf = 2^xf, error < 2.3e-16
	double xf = x - xe;
	double yf = ((((((((((6.302690884e-10*xf + 6.537941907e-09)*xf + 1.025834708428e-07)*xf + 1.32076762706e-06)*xf + 1.5253232908459e-05)*xf + 0.000154035091891941)*xf + 0.0013333558738165096)*xf + 0.0096181290971755593)*xf + 0.055504108665909871)*xf + 0.24022650695904222)*xf + 0.69314718055994654)*xf + 1.;

	return ye * yf;
}

double log(double x) { return log2(x) * 0.6931471805599453; }
double log2(double x)	// error < 5.6e-16 in x = [1 , 2]
{
	if (x == 0.) return -INFINITY;
	if (isfinite(x) == 0 || x < 0.)	return NAN;
	int dexp = ((double_as_int(x) >> 52) & 0x7FF) - 1023;
	x = int_as_double(0x3FF0000000000000 | (double_as_int(x) & 0x000FFFFFFFFFFFFF)) - 1.;	// x -> [0 , 1[
	double mlog = (((((((((((((((((((-3.875403760417e-05*x + 0.00044698118761163)*x - 0.00244848531481462)*x + 0.00852083615839964)*x - 0.0213191869638036)*x + 0.04131053521866153)*x - 0.065555177283666503)*x + 0.089482861465356655)*x - 0.11005436646074359)*x + 0.12723476135812695)*x - 0.14305519031975983)*x + 0.16000129093273448)*x - 0.18027986442557736)*x + 0.20609098709475556)*x - 0.24044827892686577)*x + 0.28853893972854774)*x - 0.36067375670991322)*x + 0.48089834685222948)*x - 0.72134752044262604)*x + 1.442695040888951)*x;
	return (double) dexp + mlog;
}
double log10(double x) { return log2(x) * 0.3010299956639812; }
double pow(double x, double y) { return exp2(log2(x) * y); }
double cbrt(double x) { return pow(x, 1./3.); }

double sin(double x) { return sin_tr(x * (1./(2.*M_PI))); }
double cos(double x) { return cos_tr(x * (1./(2.*M_PI))); }
double sin_tr(double x) { return cos_tr(x - 0.25); }
double cos_tr(double x)	// error < 3.4e-16
{
	x = fabs(x - floor(x) - 0.5) - 0.25;	// x --> [-0.25 , 0.25]
	double x2 = x * x;
	return ((((((((0.1007146753*x2 - 0.7176853699)*x2 + 3.81992279752)*x2 - 15.0946413686846)*x2 + 42.05869391526577)*x2 - 76.705859752634335)*x2 + 81.60524927607172)*x2 - 41.341702240399748)*x2 + 6.2831853071795865)*x;
}

double tan(double x) { return sin(x) / cos(x); }
double atan(double x) { return atan2(x, 1.); }
float atan2f(float y, float x) { return atan2(y, x); }
double atan2(double y, double x)	// error < 4.5e-16 radians
{
	double xa = fabs(x), ya = fabs(y);
	double z = (ya-xa) / (ya+xa);
	double z2 = z * z;
	z = ((((((((((((((((((((1.0855325905493e-05*z2 - 0.000127895606647823)*z2 + 0.000716185939732283)*z2 - 0.002548984741415465)*z2 + 0.006522051887574913)*z2 - 0.012912646727945627)*z2 + 0.020892606648891447)*z2 - 0.028973618243504723)*z2 + 0.036024498001997648)*z2 - 0.041870772536141579)*z2 + 0.047092469780129998)*z2 - 0.05249366638684719)*z2 + 0.058795099763367187)*z2 - 0.066662143709156916)*z2 + 0.076922535641828372)*z2 - 0.09090904383293701)*z2 + 0.11111110827526765)*z2 - 0.14285714274686638)*z2 + 0.19999999999752311)*z2 - 0.333333333333307)*z2 + 0.99999999999999992)*z + 0.78539816339744831;
	if (x < 0.)	z = M_PI - z;
	if (y < 0.)	z = -z;
	return z;
}

float asinf(float x) { return asin(x); }
double asin(double x)	// error < 7e-16
{
	double y, xa = fabs(x);
	double xm = 1. - sqrt(1. - xa);
	if (xm > 0.6)
		y = (((((((((((-0.00184090735900975*xm + 0.0204313986896723)*xm - 0.1052467657067292)*xm + 0.335492459818416)*xm - 0.7435462926619241)*xm + 1.2237228343719441)*xm - 1.5646256928232335)*xm + 1.6235101917880144)*xm - 1.4315658943509152)*xm + 1.1918839257926788)*xm - 0.97470740300978645)*xm + 1.9971348885324504)*xm + 0.00015358371331862;
	else
	{
		y = ((((0.2003*xm - 0.6249)*xm + 0.94057)*xm - 0.93961)*xm + 1.9963)*xm;
		for (int i=0; i < 2; i++)	// Newton-Raphson steps
		{
			double s = sin(y);
			double c = sqrt(1. - s*s);
			y -= (s - xa) / c;
		}
	}
	return copysign(y, x);
}

double acos(double x) { return 0.5*M_PI - asin(x); }
double sinh(double x) { return (exp(x) - exp(-x)) * 0.5; }
double cosh(double x) { return (exp(x) + exp(-x)) * 0.5; }
double tanh(double x) { double e = exp(2.*x); return (e-1.) / (e+1.); }
double hypot(double x, double y) { return sqrt(x*x + y*y); }
double tgamma(double x) { return NAN; }	// TODO

double erf(double x)	// error < 1.5e-15
{
	double y, xa = fabs(x);
	if (xa > 6.)
		return copysign(1., x);
	y = (((((((((((((((((((((3.847446425233e-15*xa - 1.67842955663171e-13)*xa + 3.477665942359862e-12)*xa - 4.5278406665374846e-11)*xa + 4.1431968115923484e-10)*xa - 2.8191376405557979e-09)*xa + 1.4694293215089563e-08)*xa - 5.9193430766414834e-08)*xa + 1.817739597449057e-07)*xa - 3.9725626384852649e-07)*xa + 4.713180470756501e-07)*xa + 4.889167169868577e-07)*xa - 3.217811989057157e-06)*xa + 6.5040168714717147e-06)*xa + 1.641819587052259e-05)*xa - 2.883536587780375e-05)*xa + 0.00034381014214142996)*xa + 0.0012709605848389835)*xa + 0.0033952701135075287)*xa + 0.024538445328735638)*xa + 0.08952465548981831)*xa + 0.14104739588692786)*xa + 1.;
	y = y*y; y = y*y; y = y*y;	// y = 1 - y^-8
	return copysign(1. - 1./y, x);
}


//**** ctype.h ****

int isspace(int c) { return c == ' ' || (unsigned)c-'\t' < 5; }
int isdigit(int c) { return (unsigned)c-'0' < 10; }
int isxdigit(int c) { return isdigit(c) || ((unsigned)c|32)-'a' < 6; }
int isalpha(int c) { return ((unsigned)c|32)-'a' < 26; }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int isupper(int c) { return (unsigned)c-'A' < 26; }
int tolower(int c) { if (isupper(c)) return c | 32; return c; }


//**** stdio.h ****

FILE *const stdin = NULL;
FILE *const stdout = NULL;
FILE *const stderr = NULL;


//**** stdlib.h ****

int abs(int j) { return j > 0 ? j : -j; }
long long int llabs(long long int j) { return j > 0 ? j : -j; }
int atoi(const char *nptr) { int v=0; sscanf(nptr, "%d", &v); return v; }
double atof(const char *nptr) { double v=0; sscanf(nptr, "%lg", &v); return v; }

double strtod(const char *nptr, char **endptr)
{
	double v = NAN;
	int n = 0;
	sscanf(nptr, "%lg%n", &v, &n);
	*endptr = (char *) &nptr[n];
	return v;
}

static uint64_t libc_rand_seed;
void srand(unsigned int seed) { libc_rand_seed = seed - 1; }
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
	for (gap = 0; ++gap < nmemb;) gap *= 3;
	while ((gap /= 3) != 0)
	{
		wgap = size * gap;
		for (i = wgap; i < wnel; i += size)
			for (j = i - wgap; ;j -= wgap)
			{
				a = j + (char *)base;
				b = a + wgap;
				if ((*compar)(a, b) <= 0) break;
				k = size;
				do { tmp = *a; *a++ = *b; *b++ = tmp; } while (--k);
				if (j < wgap) break;
			}
	}
}


//**** string.h ****

#define BULK_MEMORY_THRESHOLD 4  // 32 in WASI libc

void *memset(void *dest, int c, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD) return __builtin_memset(dest, c, n);
#endif
	char *s = dest;
	for (; n; n--, s++) *s = c;
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD) return __builtin_memcpy(dest, src, n);
#endif
	char *d = dest;
	const char *s = src;
	for (; n; n--) *d++ = *s++;
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
#if defined(__wasm_bulk_memory__)
	if (n > BULK_MEMORY_THRESHOLD) return __builtin_memmove(dest, src, n);
#endif
	char *d = dest;
	const char *s = src;
	if (d == s) return d;
	if ((uintptr_t)s-(uintptr_t)d-n <= -2*n) return memcpy(d, s, n);
	if (d < s) for (; n; n--) *d++ = *s++;
	else while (n) n--, d[n] = s[n];
	return dest;
}

size_t strlen(const char *s)
{
	const char *e;
	for (e=s; *e; e++) {}
	return e-s;
}

void *memchr(const void *s, int c, size_t n)
{
	const unsigned char *sc = s;
	c = (unsigned char)c;
	for (; n && *sc != c; sc++, n--);
	return n ? (void *)sc : 0;
}

void *memrchr(const void *s, int c, size_t n)
{
	const unsigned char *sc = s;
	c = (unsigned char) c;
	while (n--)
		if (sc[n] == c)
			return (void *)(sc+n);
	return NULL;
}

char *strchr(const char *s, int c)
{
	c = (unsigned char)c;
	if (!c) return (char *)s + strlen(s);
	for (; *s && *(unsigned char *)s != c; s++);
	return *(unsigned char *)s == (unsigned char)c ? (char *)s : 0;
}

char *strrchr(const char *s, int c) { return memrchr(s, c, strlen(s) + 1); }

char *strpbrk(const char *s1, const char *s2) {
	size_t i1, i2;
	for (i1 = 0; s1[i1]; i1++) for (i2 = 0; s2[i2]; i2++) if (s1[i1] == s2[i2]) return (char *) &s1[i1];
	return (char *) &s1[i1];
}

char *strstr(const char *s1, const char *s2) 
{
	char c, sc;
	if ((c = *s2++) != 0)
	{
		size_t len = strlen(s2);
		do { do { if ((sc = *s1++) == 0) return NULL; } while (sc != c); } while (strncmp(s1, s2, len) != 0);
		s1--;
	}
	return (char *) s1;
}

int strcmp(const char *s1, const char *s2)
{
	for (; *s1==*s2 && *s1; s1++, s2++);
	return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	const unsigned char *l=(void *)s1, *r=(void *)s2;
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *l=s1, *r=s2;
	for (; n && *l == *r; n--, l++, r++);
	return n ? *l-*r : 0;
}

char *strcpy(char *s1, const char *s2) { return strncpy(s1, s2, SIZE_MAX); }
char *strncpy(char *s1, const char *s2, size_t n)
{
	for (; n && (*s1=*s2); n--, s2++, s1++) {}
	if (n) *s1 = '\0';
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

extern void __wasm_call_ctors(void);
__attribute__((export_name("_initialize"))) void _initialize(void)
{
	static volatile int initialized = 0;
	if (initialized) __builtin_trap();
	initialized = 1;
	__wasm_call_ctors();
}

#endif // MINQND_LIBC_IMPLEMENTATION
