#define DECL_INT_ARRAY(a) int a[2]
#define INIT_DECLTR(a, b) = { \
    a, \
    b, \
}
#define SEMICOLON ;

DECL_INT_ARRAY(array) INIT_DECLTR(-1, 1) SEMICOLON
