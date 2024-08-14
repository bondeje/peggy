int a = (T) * x;

typedef double d;   // d is a new typedef name
d d;                // first d is typedef name, second d is identifier 
auto d = 3.5;       // should be d as an identifier and not typedef name

// from https://eli.thegreenplace.net/2007/11/24/the-context-sensitivity-of-cs-grammar/

int main(void) {
    
    {
        T (x); // calling function T with argument x
        typedef int T;
        T (x); // declaration of variable x with type T
    }
    
    {
        func((T) * x); // call function 'func' with argument that is multiplication of T with x
        typedef int T;
        func((T) * x); // call function 'func' with argument that is dereference of x type-casted to T
    }
    return 0;
}
