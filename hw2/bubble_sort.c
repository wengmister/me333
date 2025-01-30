#include <stdio.h>
#include <string.h>

void bubbleSort(char arr[]) {
    int n = strlen(arr);  // Str length
    char temp;
    
    // Let's sort
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

int main() {
    char str[100];
    
    printf("Enter a string to be sorted: ");
    fgets(str, sizeof(str), stdin);
    
    str[strcspn(str, "\n")] = '\0';
    
    printf("Original String: %s\n", str);
    
    bubbleSort(str);
    
    printf("Sorted String: %s\n", str);
    
    return 0;
}
