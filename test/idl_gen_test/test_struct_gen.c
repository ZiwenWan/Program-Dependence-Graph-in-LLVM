struct person {
    int age;
    char *name;
} person;

struct t {
    int (*init)(struct person *p);
    void (*uninit)(struct person *p);
} t;

struct tt {
    const struct t *T;
} tt;

int main() {

}
