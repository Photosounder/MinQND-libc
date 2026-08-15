#ifndef MINQND_LIBC_H
#define MINQND_LIBC_H

#if defined(__wasm__) && defined(WAHE_MODULE)
#define WAHE_IO
#endif

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
#define PRId64 __PRI64 "d"
#define PRIx64 __PRI64 "x"
#define PRIX64 __PRI64 "X"
#define PRIdMAX PRId64
#define PRIxMAX PRIx64

#define SCNu8 "hhu"


//**** math.h ****

#define NAN (0./0.)
#define INFINITY (1./0.)
#define M_PI 3.141592653589793

static inline uint32_t float_as_int(float f)   { union {float f;  uint32_t i;} u; u.f = f; return u.i; }
static inline uint64_t double_as_int(double f) { union {double f; uint64_t i;} u; u.f = f; return u.i; }
static inline float int_as_float(uint32_t i)   { union {float f;  uint32_t i;} u; u.i = i; return u.f; }
static inline double int_as_double(uint64_t i) { union {double f; uint64_t i;} u; u.i = i; return u.f; }
#define isnan(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_int(x) & 0x7fffffff) > 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_int(x) & -1ULL>>1) > 0x7ffULL<<52 : 0)
#define isfinite(x) ( \
		sizeof(x) == sizeof(float) ? (float_as_int(x) & 0x7fffffff) < 0x7f800000 : \
		sizeof(x) == sizeof(double) ? (double_as_int(x) & -1ULL>>1) < 0x7ffULL<<52 : 1)
#define signbit(x) ( \
	sizeof(x) == sizeof(float) ? (int)(float_as_int(x)>>31) : \
	sizeof(x) == sizeof(double) ? (int)(double_as_int(x)>>63) : 0)

extern double fmod(double x, double y);
extern double exp(double x);
extern float expf(float x);
extern double exp2(double x);
extern double log(double x);
extern double log2(double x);
extern double log10(double x);
extern double pow(double x, double y);
extern double cbrt(double x);
extern float sinf(float x);
extern float cosf(float x);
extern float sin_trf(double x);
extern float cos_trf(double x);
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
extern float hypotf(float x, float y);
extern double tgamma(double x);
extern double erf(double x);
extern double fmin(double x, double y);
extern double fmax(double x, double y);
extern float fminf(float x, float y);
extern float fmaxf(float x, float y);
extern double frexp(double x, int *exponent);
extern float frexpf(float x, int *exponent);

#ifdef __wasm__
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
static double round(double x) { return __builtin_nearbyint(x); }
static long lroundf(float x) { return (long) __builtin_nearbyintf(x); }
static double fma(double x, double y, double z)
{
#if defined(__has_builtin) && __has_builtin(__builtin_wasm_relaxed_madd_f64x2)
	__attribute__((__vector_size__(2 * sizeof(double)))) double r2, x2 = {x}, y2 = {y}, z2 = {z};
	r2 = __builtin_wasm_relaxed_madd_f64x2(x2, y2, z2);
	return r2[0];
#else
	return x*y + z;
#endif
}

static float fmaf(float x, float y, float z)
{
#if defined(__has_builtin) && __has_builtin(__builtin_wasm_relaxed_madd_f32x4)
	__attribute__((__vector_size__(4 * sizeof(float)))) float r4, x4 = {x}, y4 = {y}, z4 = {z};
	r4 = __builtin_wasm_relaxed_madd_f32x4(x4, y4, z4);
	return r4[0];
#else
	return x*y + z;
#endif
}
#else
extern float fabsf(float x);
extern double fabs(double x);
extern float sqrtf(float x);
extern double sqrt(double x);
extern float copysignf(float x, float y);
extern double copysign(double x, double y);
extern float ceilf(float x);
extern double ceil(double x);
extern float floorf(float x);
extern double floor(double x);
extern float truncf(float x);
extern double trunc(double x);
extern float nearbyintf(float x);
extern double nearbyint(double x);
extern float rintf(float x);
extern double rint(double x);
extern double round(double x);
extern long lroundf(float x);
extern float fmaf(float x, float y, float z);
extern double fma(double x, double y, double z);
#endif


//**** ctype.h ****

extern int isspace(int c);
extern int isdigit(int c);
extern int isxdigit(int c);
extern int isalpha(int c);
extern int isalnum(int c);
extern int islower(int c);
extern int isupper(int c);
extern int toupper(int c);
extern int tolower(int c);
extern int isprint(int c);


//**** stdio.h ****

#include <stddef.h>	// for NULL and size_t
#define EOF (-1)
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define va_list		__builtin_va_list
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#ifdef __wasm__
typedef uint8_t minqnd_file_v128_t __attribute__((__vector_size__(16), __aligned__(16)));
#endif

typedef struct
{
	#ifdef __wasm__
	minqnd_file_v128_t cache[64];
	#else
	uint8_t cache[1024];
	#endif
	uint64_t position, cache_offset;
	size_t handle, cache_size;
	unsigned int mode;
	unsigned char cache_eof, eof, error;
} FILE;
extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;
extern char *fgets(char *s, int n, FILE *stream);
extern int fputs(const char *s, FILE *stream);
extern int fprintf(FILE *stream, const char *format, ...);
extern FILE *fopen(const char *filename, const char *mode);
extern int fclose(FILE *stream);
extern int feof(FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
extern int fseek(FILE *stream, long int offset, int whence);
extern long int ftell(FILE *stream);
extern void rewind(FILE *stream);

extern void *memset(void *dest, int c, size_t n);
extern void *memcpy(void *dest, const void *src, size_t n);
extern void *memmove(void *dest, const void *src, size_t n);
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
extern char *strcat(char *s1, const char *s2);
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
extern void abort(void);


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

#if INTPTR_MAX == INT64_MAX
typedef long long ssize_t;
#else
typedef long ssize_t;
#endif

#ifdef WAHE_IO
__attribute__((__import_name__("wahe_run_command"))) extern char *wahe_run_command(char *command);
#define CITA_EXCLUDE_STRING_H
#include <cita_os.h>
#endif

#endif // MINQND_LIBC_H


#ifdef MINQND_LIBC_IMPLEMENTATION

#ifdef WAHE_IO
#define CITA_OS_IMPLEMENTATION_PART1
#include <cita_os.h>
#undef CITA_OS_IMPLEMENTATION_PART1
#define CITA_OS_IMPLEMENTATION
#include <cita_os.h>
#undef CITA_OS_IMPLEMENTATION
#endif

//**** math.h ****

double fmod(double x, double y) { return x - trunc(x / y) * y; }

double frexp(double x, int *e)
{
	int er = double_as_int(x) << 1 >> 53;

	if (er == 0 && (double_as_int(x) & 0x000fffffffffffff))	// Subnormals
	{
		x *= 0x1p54;
		*e = (int) (double_as_int(x) << 1 >> 53) - (1022+54);
	}
	else if (er == 0 || er == 0x7ff)			// Zero and NaN
	{
		*e = 0;
		return x;
	}
	else
		*e = er - 1022;

	// Replace the exponent while retaining the sign and significand
	return int_as_double((double_as_int(x) & 0x800fffffffffffff) | 0x3fe0000000000000);
}

float frexpf(float x, int *e)
{
	int er = float_as_int(x) << 1 >> 24;

	if (er == 0 && (float_as_int(x) & 0x007fffff))
	{
		x *= 0x1p25f;
		*e = (int) (float_as_int(x) << 1 >> 24) - (126+25);
	}
	else if (er == 0 || er == 0xff)
	{
		*e = 0;
		return x;
	}
	else
		*e = er - 126;

	return int_as_float((float_as_int(x) & 0x807fffff) | 0x3f000000);
}

double exp(double x) { return exp2(x*1.4426950408889634); }
float expf(float x) { return exp(x); }
double exp2(double x)
{
	if (x < -1022.)	return 0.;
	if (x > 1024.)	return INFINITY;

	// Calculate the integer exponent part so that ye = 2^xe
	double xe = floor(x);
	double ye = int_as_double(((int64_t) xe + 1023) << 52);

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

float sinf(float x) { return sin_trf((double) x * (1./(2.*M_PI))); }
float cosf(float x) { return cos_trf((double) x * (1./(2.*M_PI))); }
float sin_trf(double x) { return cos_trf(x - 0.25); }
float cos_trf(double x)
{
	x = fabs(x - floor(x) - 0.5) - 0.25;	// x --> [-0.25 , 0.25]
	double x2 = x * x;
	return ((((39.53581*x2 - 76.54966)*x2 +	81.601)*x2 - 41.34165)*x2 + 6.283185)*x;
}

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
float hypotf(float x, float y) { return sqrtf(x*x + y*y); }
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

#define _MINN(a, b) ((a) < (b) ? (a) : (b))
#define _MAXN(a, b) ((a) > (b) ? (a) : (b))

double fmin(double x, double y)
{
	if (isnan(x)) return y;
	if (isnan(y)) return x;
	if (signbit(x) != signbit(y)) return signbit(x) ? x : y;
	return _MINN(x, y);
}

double fmax(double x, double y)
{
	if (isnan(x)) return y;
	if (isnan(y)) return x;
	if (signbit(x) != signbit(y)) return signbit(x) ? y : x;
	return _MAXN(x, y);
}

float fminf(float x, float y)
{
	if (isnan(x)) return y;
	if (isnan(y)) return x;
	if (signbit(x) != signbit(y)) return signbit(x) ? x : y;
	return _MINN(x, y);
}

float fmaxf(float x, float y)
{
	if (isnan(x)) return y;
	if (isnan(y)) return x;
	if (signbit(x) != signbit(y)) return signbit(x) ? y : x;
	return _MAXN(x, y);
}

#undef _MINN
#undef _MAXN


//**** ctype.h ****

int isspace(int c) { return c == ' ' || (unsigned)c-'\t' < 5; }
int isdigit(int c) { return (unsigned)c-'0' < 10; }
int isxdigit(int c) { return isdigit(c) || ((unsigned)c|32)-'a' < 6; }
int isalpha(int c) { return ((unsigned)c|32)-'a' < 26; }
int isalnum(int c) { return isalpha(c) || isdigit(c); }
int islower(int c) { return (unsigned)c-'a' < 26; }
int isupper(int c) { return (unsigned)c-'A' < 26; }
int toupper(int c) { if (islower(c)) return c & ~32; return c; }
int tolower(int c) { if (isupper(c)) return c | 32; return c; }
int isprint(int c) { return (unsigned)c-0x20 < 0x5f; }


//**** stdio.h ****

FILE *const stdin = NULL;
FILE *const stdout = NULL;
FILE *const stderr = NULL;

#ifdef WAHE_IO
#define MINQND_FILE_CACHE_SIZE 1024
#define MINQND_FILE_READ 1
#define MINQND_FILE_WRITE 2
#define MINQND_FILE_APPEND 4

static unsigned int minqnd_file_parse_mode(const char *mode)
{
	// Validate the required leading access mode
	if (mode == NULL || (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a'))
		return 0;

	// Derive the basic access flags
	unsigned int flags = mode[0] == 'r' ? MINQND_FILE_READ : MINQND_FILE_WRITE;
	if (mode[0] == 'a')
		flags |= MINQND_FILE_APPEND;

	// Accept the standard binary and update suffixes in either order
	int binary_seen = 0, update_seen = 0;
	for (size_t i=1; mode[i]; i++)
	{
		if (mode[i] == 'b' && binary_seen == 0)
			binary_seen = 1;
		else if (mode[i] == '+' && update_seen == 0)
		{
			update_seen = 1;
			flags |= MINQND_FILE_READ | MINQND_FILE_WRITE;
		}
		else
			return 0;
	}
	return flags;
}

static int minqnd_file_fill_cache(FILE *stream)
{
	char command[192];

	// Discard the previous window before requesting a replacement
	stream->cache_size = 0;
	stream->cache_eof = 0;
	stream->cache_offset = stream->position;
	int command_len = snprintf(command, sizeof(command),
		"Load from file %zu at %#" PRIx64 " to location: %d bytes at %#zx",
		stream->handle, stream->position, MINQND_FILE_CACHE_SIZE, (size_t) (uintptr_t) stream->cache);
	if (command_len < 0 || (size_t) command_len >= sizeof(command))
	{
		stream->error = 1;
		return 0;
	}

	// Run the synchronous load command and require a result message
	char *response = wahe_run_command(command);
	if (response == NULL)
	{
		stream->error = 1;
		return 0;
	}

	// Parse the byte count and the optional terminal condition
	size_t loaded = 0;
	int response_len = 0, valid = sscanf(response, "Loaded %zu bytes%n", &loaded, &response_len) == 1;
	const char *suffix = valid ? &response[response_len] : "";
	if (!valid || loaded > MINQND_FILE_CACHE_SIZE || stream->cache_offset > UINT64_MAX - loaded)
		stream->error = 1;
	else if (suffix[0] == '\0')
	{
		if (loaded == 0)
			stream->error = 1;
	}
	else if (strcmp(suffix, " (end of file)") == 0)
	{
		stream->cache_eof = 1;
		if (loaded == 0)
			stream->eof = 1;
	}
	else if (strcmp(suffix, " (read error)") == 0)
		stream->error = 1;
	else
		stream->error = 1;

	// Keep only data described by a valid response
	if (valid && loaded <= MINQND_FILE_CACHE_SIZE && stream->cache_offset <= UINT64_MAX - loaded)
		stream->cache_size = loaded;
	free(response);
	return stream->cache_size != 0;
}

static int minqnd_file_get_size(FILE *stream, uint64_t *file_size)
{
	char command[96];

	// Request the current size from the file service
	int command_len = snprintf(command, sizeof(command), "Get size of file %zu", stream->handle);
	if (command_len < 0 || (size_t) command_len >= sizeof(command))
	{
		stream->error = 1;
		return 0;
	}
	char *response = wahe_run_command(command);
	if (response == NULL)
	{
		stream->error = 1;
		return 0;
	}

	// Parse the complete decimal size response
	int response_len = 0;
	intmax_t parsed_size = -1;
	int valid = sscanf(response, "File size %ju bytes%n", &parsed_size, &response_len) == 1 &&
		parsed_size >= 0 && response[response_len] == '\0';
	if (valid)
		*file_size = (uint64_t) parsed_size;
	else
		stream->error = 1;
	free(response);
	return valid;
}

static int minqnd_file_add_offset(uint64_t base, long int offset, uint64_t *result)
{
	// Add nonnegative offsets without wrapping
	if (offset >= 0)
	{
		uint64_t positive_offset = (uint64_t) offset;
		if (base > UINT64_MAX - positive_offset)
			return 0;
		*result = base + positive_offset;
		return 1;
	}

	// Subtract negative offsets without negating the minimum long value
	uint64_t negative_offset = 0 - (uint64_t) offset;
	if (negative_offset > base)
		return 0;
	*result = base - negative_offset;
	return 1;
}
#endif

char *fgets(char *s, int n, FILE *stream)
{
#ifdef WAHE_IO
	// Reject unusable destinations and streams
	if (s == NULL || n <= 0 || stream == NULL || (stream->mode & MINQND_FILE_READ) == 0)
		return NULL;

	// Read through the shared cache until a newline or the destination limit
	int pos = 0;
	while (pos < n-1)
	{
		if (fread(&s[pos], 1, 1, stream) != 1)
			break;
		if (s[pos++] == '\n')
			break;
	}
	if (pos == 0 && n > 1)
		return NULL;
	s[pos] = '\0';
	return s;
#else
	(void) s;
	(void) n;
	(void) stream;
	return NULL;
#endif
}

int fprintf(FILE *stream, const char *format, ...)
{
#ifdef WAHE_IO
	// Reject invalid output streams before consuming arguments
	if (stream == NULL || format == NULL || (stream->mode & MINQND_FILE_WRITE) == 0)
		return -1;

	// Measure the formatted output with a copied argument list
	va_list args, measure_args;
	va_start(args, format);
	va_copy(measure_args, args);
	int length = vsnprintf(NULL, 0, format, measure_args);
	va_end(measure_args);
	if (length < 0)
	{
		va_end(args);
		stream->error = 1;
		return -1;
	}

	// Reuse the stream cache or allocate enough temporary space
	char *output = (char *) stream->cache;
	int allocated = (size_t) length >= sizeof(stream->cache);
	stream->cache_size = 0;
	stream->cache_eof = 0;
	if (allocated)
		output = malloc((size_t) length + 1);
	if (output == NULL)
	{
		va_end(args);
		stream->error = 1;
		return -1;
	}

	// Format once into the selected buffer and write every produced byte
	int formatted = vsnprintf(output, (size_t) length + 1, format, args);
	va_end(args);
	size_t written = formatted == length ? fwrite(output, 1, (size_t) length, stream) : 0;
	if (allocated)
		free(output);
	if (formatted != length || written != (size_t) length)
	{
		stream->error = 1;
		return -1;
	}
	return length;
#else
	(void) stream;
	(void) format;
	return -1;
#endif
}

FILE *fopen(const char *filename, const char *mode)
{
#ifdef WAHE_IO
	// Validate locally representable modes and command paths
	unsigned int flags = minqnd_file_parse_mode(mode);
	if (filename == NULL || flags == 0 || strchr(filename, '\n') || strchr(filename, '\r'))
		return NULL;

	// Allocate the stream before opening its native handle
	FILE *stream = calloc(1, sizeof(*stream));
	if (stream == NULL)
		return NULL;
	int command_len = snprintf(NULL, 0, "Open file (%s) at %s", mode, filename);
	if (command_len < 0)
	{
		free(stream);
		return NULL;
	}
	char *command = malloc((size_t) command_len + 1);
	if (command == NULL)
	{
		free(stream);
		return NULL;
	}
	snprintf(command, (size_t) command_len + 1, "Open file (%s) at %s", mode, filename);

	// Open the native file and parse its nonzero opaque handle
	char *response = wahe_run_command(command);
	free(command);
	int response_len = 0;
	size_t handle = 0;
	int valid = response && sscanf(response, "File handle %zu%n", &handle, &response_len) == 1 &&
		handle != 0 && response[response_len] == '\0';
	if (response)
		free(response);
	if (!valid)
	{
		free(stream);
		return NULL;
	}

	// Initialise the module-side logical stream state
	stream->handle = handle;
	stream->mode = flags;
	return stream;
#else
	(void) filename;
	(void) mode;
	return NULL;
#endif
}

int fclose(FILE *stream)
{
#ifdef WAHE_IO
	// Reject streams without a live native handle
	if (stream == NULL || stream->handle == 0)
		return EOF;
	size_t handle = stream->handle;
	char command[96];
	int command_len = snprintf(command, sizeof(command), "Close file %zu", handle);

	// Close the native handle and validate its acknowledgement
	char *response = command_len >= 0 && (size_t) command_len < sizeof(command) ? wahe_run_command(command) : NULL;
	int response_len = 0;
	size_t closed_handle = 0;
	int valid = response && sscanf(response, "Closed file %zu%n", &closed_handle, &response_len) == 1 &&
		closed_handle == handle && response[response_len] == '\0';
	if (response)
		free(response);

	// Release the module-side stream regardless of the close result
	stream->handle = 0;
	free(stream);
	return valid ? 0 : EOF;
#else
	(void) stream;
	return EOF;
#endif
}

int feof(FILE *stream)
{
#ifdef WAHE_IO
	// Report whether a previous logical read encountered the end of the stream
	return stream != NULL && stream->eof != 0;
#else
	(void) stream;
	return 0;
#endif
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
#ifdef WAHE_IO
	// Handle empty requests before validating their unused destination
	if (size == 0 || nmemb == 0)
		return 0;
	if (ptr == NULL || stream == NULL || (stream->mode & MINQND_FILE_READ) == 0)
		return 0;
	size_t requested = 0;
	if (__builtin_mul_overflow(size, nmemb, &requested))
	{
		stream->error = 1;
		return 0;
	}

	// Consume cached bytes and synchronously refill only when necessary
	size_t copied = 0;
	while (copied < requested)
	{
		if (stream->position >= stream->cache_offset)
		{
			uint64_t cache_pos = stream->position - stream->cache_offset;
			if (cache_pos < stream->cache_size)
			{
				size_t available = stream->cache_size - (size_t) cache_pos;
				size_t copy_size = requested - copied < available ? requested - copied : available;
				memcpy(&((uint8_t *) ptr)[copied], &((uint8_t *) stream->cache)[cache_pos], copy_size);
				stream->position += copy_size;
				copied += copy_size;
				continue;
			}
			if (cache_pos == stream->cache_size && stream->cache_eof)
			{
				stream->eof = 1;
				break;
			}
		}
		if (stream->error || !minqnd_file_fill_cache(stream))
			break;
	}
	return copied / size;
#else
	(void) ptr;
	(void) size;
	(void) nmemb;
	(void) stream;
	return EOF;
#endif
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
#ifdef WAHE_IO
	// Handle empty requests before validating their unused source
	if (size == 0 || nmemb == 0)
		return 0;
	if (ptr == NULL || stream == NULL || (stream->mode & MINQND_FILE_WRITE) == 0)
		return 0;
	size_t requested = 0;
	if (__builtin_mul_overflow(size, nmemb, &requested))
	{
		stream->error = 1;
		return 0;
	}

	// Expose the source range to the native file service
	char command[224];
	int command_len = snprintf(command, sizeof(command),
		"Write to file %zu at %#" PRIx64 " from location: %zu bytes at %#zx",
		stream->handle, stream->position, requested, (size_t) (uintptr_t) ptr);
	stream->cache_size = 0;
	stream->cache_eof = 0;
	stream->eof = 0;
	if (command_len < 0 || (size_t) command_len >= sizeof(command))
	{
		stream->error = 1;
		return 0;
	}
	char *response = wahe_run_command(command);
	if (response == NULL)
	{
		stream->error = 1;
		return 0;
	}

	// Parse the written byte count, resulting position and optional error
	size_t written = 0;
	int response_len = 0;
	intmax_t parsed_position = -1;
	int valid = sscanf(response, "Wrote %zu bytes, position %jx%n", &written, &parsed_position, &response_len) == 2 &&
		parsed_position >= 0 && written <= requested;
	const char *suffix = valid ? &response[response_len] : "";
	int write_error = valid && strcmp(suffix, " (write error)") == 0;
	if (!valid || (suffix[0] && !write_error))
	{
		stream->error = 1;
		written = 0;
	}
	else
	{
		stream->position = (uint64_t) parsed_position;
		if (write_error || written < requested)
			stream->error = 1;
	}
	free(response);
	return written / size;
#else
	(void) ptr;
	(void) size;
	(void) nmemb;
	(void) stream;
	return EOF;
#endif
}

int fputs(const char *s, FILE *stream)
{
	// Write the complete string without its terminator
	size_t length = strlen(s);
	return fwrite(s, 1, length, stream) == length ? 0 : EOF;
}

int fseek(FILE *stream, long int offset, int whence)
{
#ifdef WAHE_IO
	// Resolve the selected origin without changing the stream first
	if (stream == NULL || stream->handle == 0)
		return EOF;
	uint64_t base = 0, position = 0;
	if (whence == SEEK_SET)
		base = 0;
	else if (whence == SEEK_CUR)
		base = stream->position;
	else if (whence == SEEK_END)
	{
		if (!minqnd_file_get_size(stream, &base))
			return EOF;
	}
	else
	{
		stream->error = 1;
		return EOF;
	}

	// Commit a representable nonnegative logical position and clear EOF
	if (!minqnd_file_add_offset(base, offset, &position))
	{
		stream->error = 1;
		return EOF;
	}
	stream->position = position;
	stream->eof = 0;
	return 0;
#else
	(void) stream;
	(void) offset;
	(void) whence;
	return EOF;
#endif
}

long int ftell(FILE *stream)
{
#ifdef WAHE_IO
	// Return only positions representable by the public long result
	if (stream == NULL || stream->handle == 0)
		return EOF;
	long int position = (long int) stream->position;
	if (position < 0 || (uint64_t) position != stream->position)
	{
		stream->error = 1;
		return EOF;
	}
	return position;
#else
	(void) stream;
	return 0;
#endif
}

void rewind(FILE *stream)
{
#ifdef WAHE_IO
	// Seek to the start and clear both stream indicators
	if (stream)
	{
		(void) fseek(stream, 0, SEEK_SET);
		stream->eof = 0;
		stream->error = 0;
	}
#else
	(void) stream;
#endif
}

#ifdef WAHE_IO
#undef MINQND_FILE_CACHE_SIZE
#undef MINQND_FILE_READ
#undef MINQND_FILE_WRITE
#undef MINQND_FILE_APPEND
#endif


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

#if defined(__GNUC__) && __GNUC__ >= 13
  #define __builtin_assume(...) __attribute__((__assume__(__VA_ARGS__)))
#endif

void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *))
{
	size_t wnel, gap, wgap, i, j, s;
	char *a, *b, tmp;
	if (size == 0) return;
	wnel = size * nmemb;
	gap = nmemb;
	while (gap > 1)
	{
		gap = (5ull * gap - 1) / 11;
		if (gap == 0) gap = 1;

		wgap = size * gap;
		__builtin_assume(wgap < wnel);
		for (i = wgap; i < wnel; i += size)
		{
			for (j = i; !__builtin_sub_overflow(j, wgap, &j);)
			{
				a = j + (char *)base;
				b = a + wgap;
				if (compar(a, b) <= 0) break;
				s = size;
				do { tmp = *a; *a++ = *b; *b++ = tmp; } while (--s);
			}
		}
	}
}

void abort(void) { __builtin_trap(); }


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
	return NULL;
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

char *strcat(char *s1, const char *s2)
{
	strcpy(s1 + strlen(s1), s2);
	return s1;
}

char *strcpy(char *s1, const char *s2)
{
	char *d = s1;
	for (; (*d=*s2); d++, s2++) {}
	return s1;
}

char *strncpy(char *s1, const char *s2, size_t n)
{
	char *d = s1;
	for (; n && *s2; n--, s2++, d++) *d = *s2;
	for (; n; n--, d++) *d = '\0';
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

#ifdef __wasm__
extern void __wasm_call_ctors(void);
__attribute__((export_name("_initialize"))) void _initialize(void)
{
	static volatile int initialized = 0;
	if (initialized) __builtin_trap();
	initialized = 1;
	__wasm_call_ctors();
}
#endif

#endif // MINQND_LIBC_IMPLEMENTATION
