
struct Person {
    int age;
    char *name;
    union 
    {
        int a;
        int b;
    };
};

int f(struct Person* p) {
    struct Person p1 = *p;
    return 0;
}

int main() {
    struct Person p = {1, "J"};
    f(&p);
}
