
// this is a comment \
    is it line continued properly? Yes

#define A
#define B 1
#define C B

#define D int
#define E a
#define F =
#define G B \
 ;

// should result in "int a = 1 ;" tokenized
D E F G
