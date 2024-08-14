// testing non-function c declarations. only uses built-in types and keywords. Any includes are to be in a different test
_Bool _;
char a;
char * b;
const short c;
extern volatile long int d;
static unsigned long long e;
auto f = 1;
thread_local float g;
constexpr signed char h = '\''; // constexpr requires an initializer
typedef double * const i;
i j;
struct k {
    unsigned char l;
    struct k * m;
};
struct { // structs not handled yet
    signed char o;
    void * p;
} q;
typedef struct {long double r;} s; // structs not handled yet in type THIS IS WHERE I AM. NEED TO IMPLEMENT STRUCTS
s t; // type not identified yet. correctly parsed as typedef-name but no type is created
//unsigned char * u, v; // invalid in cexpress
unsigned char * u;
union {
    char v;
    short w;
};
union { // unions not handled yet
    int z;
    float aa;
} ab;
int ak[3];
short al[2][4];
double * ax[3][4];
double (*ay)[3][4];
double (*az)[][3]; // having no value is only accepted in first bracket
void am();
void an(void);
int ao(void);
int ap(int);
int * aq(int);
int * ar(int *);
s as(s *);
long long at(short au);
double av(float * aw);
int ba(void (*bb)(int *));
int (*bc(float, short))(int);
// functions that take a function as input that taks a char and returns a long, that then returns a function that takes bool and short input and returns an in
int (*bd(long (*)(char)))(bool, short); // this fails???
int (*be(long (*bf)(char)))(bool, short); // This is OK
int (*bg(long (char)))(bool, short); // this is also OK
int (*bh(long bi(char)))(bool, short); // this is also OK
int (bj(long bk(char)))(bool, short); // this is semantically invalid, must return pointers to arrays/functions, cannot return arrays/functions directly
int (*bl[4])(bool, short);
double (*bm(bool, short))[3]; // a function that takes a bool and short and returns a pointer to an array of 3 doubles
