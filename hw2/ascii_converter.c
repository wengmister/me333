#include <stdio.h>

int main() {
    printf("ASCII Table (33 to 127)\n");
    printf("------------------------\n");
    printf("Number\tCharacter\n");
    printf("------------------------\n");

    for (int i = 33; i <= 127; i++) {
        printf("%d\t%c\n", i, i);
    }

    return 0;
}
