#include <cstdio>
#include <cstdlib>
#include <unistd.h>
int main() {
    malloc(1000000);
    while (1) {
        sleep(1);
    }
}
