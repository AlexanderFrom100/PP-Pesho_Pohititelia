#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ROWS 1000
#define MAX_COLS 1000

typedef struct Graph_node {
    int size;
    double weight;
    struct Graph_node *next_node;
} Graph_node;

typedef struct Graph {
    int numVertices;
    Graph_node **adjList;
} Graph;

int rows;
int cols;
char map[MAX_ROWS][MAX_COLS];

Graph_node *init_node(int size, double weight) {
    Graph_node *node = (Graph_node *)malloc(sizeof(Graph_node));
    node->size = size;
    node->next_node = NULL;
    node->weight = weight;

    return node;
}

Graph *init_graph(int numVertices) {
    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->numVertices = numVertices;
    graph->adjList = (Graph_node **)calloc(numVertices, sizeof(Graph_node *));
    return graph;
}

void addEdgeDirectional(Graph *graph, int from, int to, double weight) {
    if(from < 0 || from >= graph->numVertices || to < 0 || to >= graph->numVertices) return;
  
    Graph_node *newNode = init_node(to, weight);

    newNode->next_node = graph->adjList[from];
    graph->adjList[from] = newNode;
}

void addEdge(Graph *graph, int from, int to, double weight) {
    addEdgeDirectional(graph, from, to, weight);
    addEdgeDirectional(graph, to, from, weight);
}

void printGraph(Graph *graph)
{
    for(int i = 0; i < graph->numVertices; i++)
    {
        Graph_node *current = graph->adjList[i];
        printf("Neighbours of %d:\n", i);
        while(current != NULL)
        {
            printf("%d[%lf] ", current->size, current->weight);
            current = current->next_node;
        }
        printf("\n");
    }
}

void read_map(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    rows = 0;
    cols = 0;
    char line[MAX_COLS];
    while(fgets(line, sizeof(line), file)) {
        int len = 0;
        for(int i = 0; line[i] != '\n' && line[i] != '\0'; i++) {
            map[rows][i] = line[i];
            len++;
        }
        if(len > cols) cols = len;
        rows++;
    }
    fclose(file);
}

double pythagoras(int a, int b) {
    return sqrt(a * a + b * b);
}

double dist(int start1, int end1, int start2, int end2) {
    if(start1 % (MAX_COLS * 10) <= end2 % (MAX_COLS * 10)) {
        if(end1 % (MAX_COLS * 10) < start2 % (MAX_COLS * 10)) {
            return pythagoras((int)((end1 % (MAX_COLS * 10) - start2 % (MAX_COLS * 10)) - (end1 % (MAX_COLS * 10) - start2 % (MAX_COLS * 10) < 0 ? -1 : 1)), (int)(end1 / (MAX_COLS * 10) - start2 / (MAX_COLS * 10)));
        } else return (start1 - start2) / (MAX_COLS * 10) < 0 ? -((start1 - start2) / (MAX_COLS * 10)) - 1 : (start1 - start2) / (MAX_COLS * 10) - 1;
    } else return pythagoras((int)((end2 % (MAX_COLS * 10) - start1 % (MAX_COLS * 10)) - (end2 % (MAX_COLS * 10) - start1 % (MAX_COLS * 10) < 0 ? -1 : 1)), (int)(end2 / (MAX_COLS * 10) - start1 / (MAX_COLS * 10)));
}

typedef struct PQNode
{
    int vertex;
    int priority;
} PQNode;

typedef struct PriorityQueue
{
    PQNode *nodes[10000];
    int size;
} PriorityQueue;

PQNode *createPQNode(int v, int p)
{
    PQNode *newNode = (PQNode *)malloc(sizeof(PQNode));
    newNode->vertex = v;
    newNode->priority = p;
    return newNode;
}

PriorityQueue *createPriorityQueue()
{
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    pq->size = 0;
    return pq;
}

void swap(PQNode **a, PQNode **b)
{
    PQNode *temp = *a;
    *a = *b;
    *b = temp;
}

void siftDown(PriorityQueue *pq, int i)
{
    int swapped = 1;

    while(swapped)
    {
        swapped = 0;
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if(left < pq->size && pq->nodes[left] < pq->nodes[smallest])
        {
            smallest = left;
        }
        if(right < pq->size && pq->nodes[right] < pq->nodes[smallest])
        {
            smallest = right;
        }

        if(i != smallest)
        {
            swap(&pq->nodes[i], &pq->nodes[smallest]);
            i = smallest;
            swapped = 1;
        }
    }
}

void insert(PriorityQueue *pq, int v, int p)
{
    PQNode *newNode = createPQNode(v, p);
    pq->nodes[pq->size] = newNode;
    int current = pq->size++;
    int parent = (current - 1) / 2;

    while(current != 0 && pq->nodes[current]->priority < pq->nodes[parent]->priority)
    {
        swap(&pq->nodes[current], &pq->nodes[parent]);
        current = parent;
        parent = (current - 1) / 2;
    }
}

PQNode *removeMin(PriorityQueue *pq)
{
    PQNode *minNode = pq->nodes[0];
    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;
    siftDown(pq, 0);
    return minNode;
}

int best_first_search(Graph *graph, int start, int end, int jump_length)
{
    if(start == end) return 1;
    PriorityQueue *open = createPriorityQueue();
    int *visited = (int *)malloc(graph->numVertices * sizeof(int));
    for(int i = 0; i < graph->numVertices; i++)
    {
        visited[i] = 0;
    }

    insert(open, start, 0);

    while(open->size != 0)
    {
        PQNode *currentNode = removeMin(open);
        int current = currentNode->vertex;
        visited[current] = 1;

        if (current == end)
        {
            return 1;
        }

        Graph_node *temp = graph->adjList[current];
        while(temp != NULL)
        {
            if(!visited[temp->size])
            {
                if(temp->weight <= jump_length)
                insert(open, temp->size, temp->weight);
            }
            temp = temp->next_node;
        }
    }
    return 0;
}


int main() {
    char filename[100];
    int pesho_jump, police_jump, pesho_start, police_start;

    printf("File path: ");
    scanf("%s", filename);

    printf("Jump distance for Pesho: ");
    scanf("%d", &pesho_jump);

    printf("Pesho start(platform): ");
    scanf("%d", &pesho_start);

    printf("Jump distance for police: ");
    scanf("%d", &police_jump);

    printf("Starting position for the police(platform): ");
    scanf("%d", &police_start);

    read_map(filename);

    int k = 0;
    int *start_p = (int *)malloc(sizeof(int));
    int *end_p = (int *)malloc(sizeof(int));

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            if(map[i][j] == '#') {
                start_p[k] = i * (MAX_COLS * 10) + j;
                while(map[i][j] == '#') j++;
                end_p[k] = i * (MAX_COLS * 10) + j - 1;
                k++;
                start_p = realloc(start_p, (k + 1) * sizeof(int));
                end_p = realloc(end_p, (k + 1) * sizeof(int));
            }
        }
    }

    Graph *graph = init_graph(k);
    for(int i = 0; i < k - 1; i++){
        for(int j = i + 1; j < k; j++){
            addEdge(graph, i, j, dist(start_p[i], end_p[i], start_p[j], end_p[j]));
        }
    }
    
    int pesho_result = 0, police_result = 0, win = 0;

    for(int i = 0; i < k; i++) {
        pesho_result = best_first_search(graph, pesho_start, i, pesho_jump);
        if(i != police_start) {
            police_result = best_first_search(graph, police_start, i, police_jump);
        } else continue;
        if(pesho_result == 1 && police_result == 0){
            printf("Pesho can run away from the police at platform at coordinates: (%d, %d)\n", start_p[i]%(MAX_COLS * 10),start_p[i]/(MAX_COLS * 10));
            win = 1;
        } 
    }
    if(!win) printf("Pesho cannot escape.\n");
    return 0;
}
