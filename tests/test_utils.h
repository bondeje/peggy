/* This is a set of utilities that can be used across test environments */

extern _Bool verbose;

#ifndef STRINGIFY
#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#endif
#ifndef CAT
#define CAT_(a, b) a##b
#define CAT(a, b) CAT_(a, b)
#endif
#ifndef CAT3
#define CAT3_(a, b, c) a##b##c
#define CAT3(a, b, c) CAT3_(a, b, c)
#endif

extern char const * test_log_file;

#define CHECK(result, failmsg, ...) check(result, "%s-%s-%zu: " failmsg, __FILE__, __func__, __LINE__, __VA_ARGS__)
int check(_Bool result, char const * failmsg, ...);

