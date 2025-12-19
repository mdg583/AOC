#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIGN(int_x) ((int_x > 0) - (int_x < 0))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Heap Relative Nodes
#define LEFT(i) (2*i + 1)
#define RIGHT(i) (2*i + 2)
#define PARENT(i) ((i-1)/2)

// Heap node
typedef struct {
    int i;
    long long value;
} Node;

int walk_h(int x1, int x2, int y, int *v_x, int *v_y, int *v_dy, Node *ind_h, Node *ind_v, int n, int overall_d);
int walk_v(int x, int y1, int y2, int *v_x, int *v_y, int *v_dx, Node *ind_h, Node *ind_v, int n, int overall_d);

// Heap functions
void h_insert(Node *heap, int n, Node x){
    // 1. Assume x is at the end of the list
    // 2. swap up unless we are at the top of the heap or would be in the wrong order relative to parent
    while(n > 0 && heap[PARENT(n)].value < x.value){
        heap[n] = heap[PARENT(n)];
        n = PARENT(n);
    }
    heap[n] = x;
}

Node h_pop(Node *heap, int n){
    // remove the root element
    Node r = heap[0];
    // Get the last element in the heap
    Node x = heap[n-1];
    n--;
    // starting at the root, either place the element (if larger than both children),
    // or swap with largest child
    // printf("\narea=%lld\n",x.area);
    int i = 0;
    while(1){
        //printf("%d -> (%d, %d) vs %d\n", i, LEFT(i), RIGHT(i), n);
        if(LEFT(i) >= n) break;
        // choose the 'largest' child. Doesn't matter if equal
        int child = (RIGHT(i) >= n) || heap[LEFT(i)].value >= heap[RIGHT(i)].value ? LEFT(i) : RIGHT(i);
        //printf("Largest child: %d\n", child);
        if(x.value >= heap[child].value) break;
        heap[i] = heap[child];
        i = child;
    }
    heap[i] = x;
    return r;
}

// assumes v_x and v_y are of size 2 (at least)
int dmotion(int dx1, int dx2, int dy1, int dy2){
    return (SIGN(dx1) * SIGN(dy2)) - (SIGN(dx2) * SIGN(dy1));
}

int cnt_hbnd(int x, int y, int *v_x, int *v_y, int n){
    // hbnd: same y
    int r = 0; int i = 0;
    if(v_y[0] != v_y[1]) i++;
    while(i < n){
        if(v_y[i] == y){
            r += (v_x[i] <= v_x[i+1]) && (v_x[i] <= x) && (x <= v_x[i+1]);
            r += (v_x[i]  > v_x[i+1]) && (v_x[i] >= x) && (x >= v_x[i+1]);
        }
        i += 2;
    }
    return r;
}

int cnt_vbnd(int x, int y, int *v_x, int *v_y, int n){
    return cnt_hbnd(y,x,v_y,v_x,n);
}

int b_search(Node *index, int n, long long value){
    // 0 1 2
    if(n==0) return 0;
    int cut = n/2;
    if(index[cut].value >= value) return b_search(index,cut,value);
    return cut + 1 + b_search(index+cut+1,n-cut-1,value);
}

int search(Node *index, int n, long long value){
    int i = b_search(index, n, value);
    if(index[i].value == value) return i;
    return -1;
}

int get_hbnd(int x, int y, int *v_x, int *v_y, Node *ind_h, int n){
    int i = search(ind_h, n/2, y);
    if(i < 0) return -1;
    while(ind_h[i].value == y){
        int j = ind_h[i].i;
        int r = 0;
        r += (v_x[j] <= v_x[j+1]) && (v_x[j] <= x) && (x <= v_x[j+1]);
        r += (v_x[j]  > v_x[j+1]) && (v_x[j] >= x) && (x >= v_x[j+1]);
        if(r > 0) return j;
        i++;
    }
    return -1;
}

int get_vbnd(int x, int y, int *v_x, int *v_y, Node *ind_v, int n){
    return get_hbnd(y, x, v_y, v_x, ind_v, n);
}

int check_overlap(int *v_x, int *v_y, int *v_dx, int *v_dy, int n){
    for(int i = 0; i < n; i++){
        if(v_dx[i] == 0){
            int x = v_x[i];
            // vertical boundary
            int dy = SIGN(v_dy[i]);
            for(int y = v_y[i]+dy; y != v_y[i+1]; y += dy){
                if(cnt_hbnd(x,y,v_x,v_y,n) + cnt_vbnd(x,y,v_x,v_y,n) > 1) return 1;
            }
        }else{
            // horizontal boundary
            int y = v_y[i];
            int dx = SIGN(v_dx[i]);
            for(int x = v_x[i]+dx; x != v_x[i+1]; x += dx){
                if(cnt_hbnd(x,y,v_x,v_y,n) + cnt_vbnd(x,y,v_x,v_y,n) > 1) return 1;
            }
        }
    }
    return 0;
}

void create_hindex(Node *index, int *v_x, int *v_y, int n){
    int i = 0;
    if(v_y[0] != v_y[1]) i++;
    while(i < n){
        index[i/2].i = i;
        index[i/2].value = v_y[i];
        i += 2;
    }
    // heapify
    for(int j = 0; j < n/2; j++) h_insert(index, j, index[j]);
    // sort, ascending order
    for(int j = (n/2-1); j >= 0; j--) {
        index[j] = h_pop(index, j+1);
    }
}

void create_vindex(Node *index, int *v_x, int *v_y, int n){
    create_hindex(index, v_y, v_x, n);
}

int main() {
    int v_x[500]; int v_y[500];
    int n = 0;
    char line[1024];
    while(fgets(line, 1024, stdin) != NULL){
        char *token;
        token = strtok(line, ",");
        v_x[n] = atoi(token);
        token = strtok(NULL, ",");
        v_y[n] = atoi(token);
        n++;
    }
    // duplicate last vertex at the end
    v_x[n] = v_x[0];
    v_y[n] = v_y[0];
    printf("Read %d lines\n", n);

    // index of horizontal boundaries, by value of y
    Node *ind_h = (Node *)malloc(sizeof(Node)*(n/2));
    Node *ind_v = (Node *)malloc(sizeof(Node)*(n/2));
    create_hindex(ind_h, v_x, v_y, n);
    create_vindex(ind_v, v_x, v_y, n);

    // Get the differences
    int v_dx[500]; int v_dy[500];
    for(int i = 0; i < n; i++){
        v_dx[i] = v_x[i+1] - v_x[i];
        v_dy[i] = v_y[i+1] - v_y[i];
    }
    v_dx[n] = v_dx[0];
    v_dy[n] = v_dy[0];

    // check if v_dx is ever 0 twice in a row
    int last_dx = v_dx[0];
    for(int i = 1; i < n; i++) {
        if(last_dx == v_dx[i]) printf("Bad!\n");
        last_dx = v_dx[i];
    }
    // confirmed that the path always 'turns' at each vertex.

    // Get the directions of each turn
    int v_d[500];
    for(int i = 0; i < n; i++){
        v_d[i] = dmotion(v_dx[i],v_dx[i+1],v_dy[i],v_dy[i+1]);
    }

    // for(int i = 0; i < n; i++){
    //     printf("%2d  :  %d,%d\n", v_d[i], v_dx[i], v_dy[i]);
    // }

    // The sum of directions should either be 4 (we made 1 clockwise motion) or -4 (we made one anti-clockwise motion)
    int overall_d = 0;
    for(int i = 0; i < n; i++) overall_d += v_d[i];
    printf("Overall rotation: %d\n", overall_d);
    overall_d = SIGN(overall_d);

    // strategy: walk around each rectangle. If we walk "off" of a boundary tile, we just need
    // to know if we walked into the 'inside' or the 'outside'. We can do this by knowing which
    // side each boundary line is "inside" vs "outside".

    // What side of each boundary line is "inside"? if overall_d > 0, inside is always "on the right"
    // If overall_d < 0, inside is always "on the left"

    // So for example, if we are on a horizonal boundary with v_dx=4, since overall_d > 0, the side
    // that is inside (+1 or -1 in y) is sign(overall_d) * sign(v_dx)

    // we will need a function to check:
    // - is the current tile on a boundary? (could it be more than one?)
    // - if we stepped off all boundaries, then for each boundary we were on, did we step "outside"?

    // Why couldn't our boundary branch like this, where X's are two overlapping boundaries:
    //   1   2
    //
    //   4   3
    //   X
    //   X
    //   5  6
    //
    //   8  7
    // In that case on an X, either side is "inside" to one boundary and "outside" to another.
    // Are there any situations where both sides are "inside"?
    //
    // 1      2
    //
    //  6 5
    //
    //  7 8
    //    X
    // 0  9
    //    4   3
    // Yikes, now either side of X is "inside". What made the difference?
    // For now, the best thing may be to "count" the number of boundaries on each tile of the
    // boundary, to verify that boundaries (in the same orientation) never overlap.

    printf("Overlap: %s\n", check_overlap(v_x,v_y,v_dx,v_dy,n) ? "yes" : "no");
    // The boundaries never overlapp.

    // create a 2-d grid of area of regions formed by pairs of vertices (with 1s on the diagonal)
    long long areas[250000];
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            long long dx = llabs(v_x[i] - v_x[j]) + 1;
            long long dy = llabs(v_y[i] - v_y[j]) + 1;
            areas[i*n+j] = dx*dy;
        }
    }

    printf("Allocating heap of size %ld\n", sizeof(Node) * (n*n));
    Node *a_heap = (Node *)malloc(sizeof(Node) * (n*n));
    for(int i = 0; i < (n*n); i++){
        a_heap[i].i = i;
        a_heap[i].value = areas[i];
    }

    // Turn a_heap into a heap
    for(int i = 0; i < (n*n); i++){
        h_insert(a_heap, i, a_heap[i]);
    }
    printf("heapified!\n");

    // just to check, get the vertexes for the 'largest' area
    // Area x = h_pop(a_heap,(n*n));
    // printf("Largest area: %d (%lld)\n", x.i, x.area);
    // int j = x.i % n;
    // int i = x.i / n;
    // printf("(%d,%d) -> %d\n", i, j, i*n+j);
    // printf("(%d,%d) -> (%d,%d)\n", v_x[i],v_y[i],v_x[j],v_y[j]);

    // Iterate over pairs of points in reverse order of area
    // For each pair of points, walk the boundary and try our algorithm

    int n2 = n*n;
    Node a_x;
    while(n2 >= 0){
        a_x = h_pop(a_heap,n2); n2--;
        int i = a_x.i / n;
        int j = a_x.i % n;
        int x1 = v_x[i]; int y1 = v_y[i];
        int x2 = v_x[j]; int y2 = v_y[j];
        // if(a_x.value > 1540000000) continue;
        // if(a_x.value > 1700000000) continue;

        if(!(n2 % 1000)) printf("\nChecking (%d,%d) - (%d,%d) : %lld ",x1,y1,x2,y2, a_x.value);

        // walk each of the 4 boundaries, checking if we step off a boundary
        if(walk_h(x1,x2,y1,v_x,v_y,v_dy,ind_h,ind_v,n,overall_d)) continue;
        if(walk_v(x1,y1,y2,v_x,v_y,v_dx,ind_h,ind_v,n,overall_d)) continue;
        if(walk_h(x2,x1,y2,v_x,v_y,v_dy,ind_h,ind_v,n,overall_d)) continue;
        if(walk_v(x2,y2,y1,v_x,v_y,v_dx,ind_h,ind_v,n,overall_d)) continue;
        break;
    }
    printf("\n");
    printf("%d: %lld\n", a_x.i, a_x.value);
    if(n2 > 0){
        printf("Largest good region: %i (%lld)\n", n2, a_x.value);
    }
    free(a_heap);
}

// Tried 1539775275: low
// Tried 2539775275: high
// Tried 1700000000: high

// 17209, 89477
//

int walk_h(int x1, int x2, int y, int *v_x, int *v_y, int *v_dy, Node *ind_h, Node *ind_v, int n, int overall_d){
    // Since we start at a vertex, we know that we start at a vertical boundary
    int bnd = get_vbnd(x1,y,v_x,v_y,ind_v,n);
    int dx = SIGN(x2-x1);
    for(int i = 0; i < abs(x2-x1); i++){
        int x = x1 + i*dx;
        // check if moving to i to i+1 takes us off the boundary
        int v_bnd = get_vbnd(x+dx,y,v_x,v_y,ind_v,n);
        if(bnd >= 0 && v_bnd == -1){
            int h_bnd = get_hbnd(x+dx,y,v_x,v_y,ind_h,n);
            if(h_bnd == -1 && SIGN(v_dy[bnd]) * SIGN(overall_d) == dx) return 1;
        }
        bnd = v_bnd;
    }
    return 0;
}

int walk_v(int x, int y1, int y2, int *v_x, int *v_y, int *v_dx, Node *ind_h, Node *ind_v,int n, int overall_d){
    return walk_h(y1,y2,x,v_y,v_x,v_dx,ind_v,ind_h,n,-overall_d);
}

// What would speed this up?
// - create a sorted index of horizontal and vertical bounds, based on the value of the 'constant' coord
// - then we can find which edges are at that coord using a binary search
