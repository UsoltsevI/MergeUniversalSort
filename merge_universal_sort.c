#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define DEBUGON

#ifdef DEBUGON
typedef int (*xcmp_t)(void *lhs, int lsz, void *rhs, int rsz);
void xmsort(void *mem, int *sizes, int nelts, xcmp_t cmp);
void xmsort_imp(void *mem, int *sizes, xcmp_t cmp, int l, int r);

int compare(void *lhs, int lsz, void *rhs, int rsz);

void print_like_char(void *mem, int sizem) {
    char *h = 0;
    for (int i = 0; i < sizem; i++) {
        h = (char *) mem + i;
        printf("'%c' ", *h);
    }

    printf("\n");
}

int main() {
    int N = 10;
#if 0
    char data[14] = {'1', '2', '3', '8', '4', '5', 'u', 'a', '-', '9', '0', 'r', 'g', 'b'};
    //partitions    | 0 |  1 |    2    |  3 |  4 |       5      |  6 |  7 |     8   |  9 |
    int sizes[10] = { 1 ,  1 ,    2    ,  1 ,  1 ,       3      ,  1 ,  1 ,     2   ,  1 };
#else 
    char data[10] = {'8', '9', '9', '7', '1', '7', '9', '9', '1', '8'};
    int sizes[10] = { 1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1 ,  1 };
#endif

    print_like_char(data, 10);

    xmsort(data, sizes, N, &compare);

    print_like_char(data, 10);

    return 0;
}

#define min(a, b) (a < b ? a : b)

int compare(void *lhs, int lsz, void *rhs, int rsz) {
    char * a = 0;
    char * b = 0;
    for (int i = min(lsz, rsz) - 1; i >= 0; i--) {
        a = (char *) (lhs + i * sizeof(char));
        b = (char *) (rhs + i * sizeof(char));
        //printf("a = %c, b = %c\n", *a, *b);
        if (*a != *b) {
            printf("%d\n", 1);
            return *a - *b;
        }
    }

    printf("%d\n", 0);

    return 0;
}

#endif 

void merge(void *mem, int *sizes, xcmp_t cmp, int l, int m, int r);

void xmsort_imp(void *mem, int *sizes, xcmp_t cmp, int l, int r) {
    int m = 0;

    if (l >= r) 
        return;
    
    m = (l + r) / 2;
    xmsort_imp(mem, sizes, cmp, l, m);
    xmsort_imp(mem, sizes, cmp, m + 1, r);
    merge(mem, sizes, cmp, l, m, r);
}

void xmsort(void *mem, int *sizes, int nelts, xcmp_t cmp) {
    xmsort_imp(mem, sizes, cmp, 0, nelts - 1);
}

int count_total_size(int *sizes, int l, int r) {
    int result = 0;

    for (int i = l; i < r; i++) {
        //printf("sizes[%d] = %d, ", i, sizes[i]);
        result += sizes[i];
    }

    //printf("\n");
    
    return result;
}

void merge(void *mem, int *sizes, xcmp_t cmp, int l, int m, int r) {
    int size_indent = count_total_size(sizes, 0, l);
    int size_first = count_total_size(sizes, l, m + 1);
    int size_second = count_total_size(sizes, m + 1, r + 1);
    int p1 = 0, p2 = 0;
    int sp1 = 0, sp2 = 0, si = size_indent;

    void *mem_first = calloc(size_first, sizeof(char));
    //void *mem_first_del = mem_first;
    int *sizes_first = (int *) calloc(m - l + 1, sizeof(int));
    //int *sizes_first_del = sizes_first;
    void *mem_second = calloc(size_first, sizeof(char));
    //void *mem_second_del = mem_second;
    int *sizes_second = (int *) calloc(r - m, sizeof(int));
    //int *sizes_second_del = sizes_second;

    

    memcpy(mem_first, mem + size_indent, size_first);
    memcpy(mem_second, mem + size_indent + size_first, size_second);

    for (int i = 0; i < m - l + 1; i++)
        sizes_first[i] = sizes[i + l];
    
    for (int i = 0; i < r - m; i++)
        sizes_second[i] = sizes[i + m + 1];

#ifdef DEBUGON
    printf("merge: \n");
    printf("sizes_first: ");
    for (int i = 0; i < m - l + 1; i++)
        printf("%d ", sizes_first[i]);
    printf("\nsizes_second: ");
    for (int i = 0; i < r - m; i++)
        printf("%d ", sizes_second[i]);
    printf("\nl = %d, m = %d, r = %d\n", l, m, r);
    printf("size_indent = %d, size_first = %d, size_second = %d\n", size_indent, size_first, size_second);
    printf("mem: ");
    print_like_char(mem + size_indent, size_first + size_second);
    printf("mem_first: ");
    print_like_char(mem_first, size_first);
    printf("mem_second: ");
    print_like_char(mem_second, size_second);
#endif

    for (int i = l; i < r + 1; i++) {
        if (((cmp(mem_first + sp1, sizes_first[p1], mem_second + sp2, sizes_second[p2]) <= 0) && 
            (sp1 < size_first)) || (sp2 >= size_second)) {
            memcpy(mem + si, mem_first + sp1, sizes_first[p1]);
            sp1 += sizes_first[p1];
            si += sizes_first[p1]; 
            sizes[i] = sizes_first[p1];
            p1++;

        } else {
            memcpy(mem + si, mem_second + sp2, sizes_second[p2]);
            sp2 += sizes_second[p2];
            si += sizes_second[p2];
            sizes[i] = sizes_second[p2];
            p2++;
        }
    } 

#ifdef DEBUGON
    printf("after merge:\n");
    print_like_char(mem + size_indent, size_first + size_second);
    printf("\n");
#endif 

    //if (mem_first_del)
        free(mem_first_del);

    //if (sizes_first_del)
        free(sizes_first_del);

    //if (mem_second_del)
        free(mem_second_del); //check if it works without free becouse there are errors in contest when i am trying to free 

    //if (sizes_second_del)
        free(sizes_second_del);
}

#undef DEBUGON
