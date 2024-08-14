struct my_type {
    double d;
    char c;
};

int main(void) {
    struct my_type a;
    a = (struct my_type){0};
    return 0;
}