#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct Vertice {
    int id;
    double sepal_length;  
    double sepal_width;   
    double petal_length;  
    double petal_width;   
    struct Vertice* proximo;
} Vertice;

typedef struct Aresta {
    int idAlvo;
    struct Aresta* proxima;
} Aresta;

typedef struct Grafo {
    int numVertices;
    Vertice* vertices;
    Aresta** listaAdjacencia;
} Grafo;

Vertice* criarVertice(int id, double sepal_length, double sepal_width, double petal_length, double petal_width) {
    Vertice* vertice = (Vertice*)malloc(sizeof(Vertice));
    vertice->id = id;
    vertice->sepal_length = sepal_length;
    vertice->sepal_width = sepal_width;
    vertice->petal_length = petal_length;
    vertice->petal_width = petal_width;
    vertice->proximo = NULL;
    return vertice;
}

void adicionarVertice(Grafo* grafo, Vertice* vertice) {
    vertice->proximo = grafo->vertices;
    grafo->vertices = vertice;
}

void adicionarAresta(Grafo* grafo, int idOrigem, int idAlvo) {
    Aresta* aresta = (Aresta*)malloc(sizeof(Aresta));
    aresta->idAlvo = idAlvo;
    aresta->proxima = grafo->listaAdjacencia[idOrigem];
    grafo->listaAdjacencia[idOrigem] = aresta;
}

Grafo* criarGrafo(int numVertices) {
    Grafo* grafo = (Grafo*)malloc(sizeof(Grafo));
    grafo->numVertices = numVertices;
    grafo->vertices = NULL;
    grafo->listaAdjacencia = (Aresta**)malloc(numVertices * sizeof(Aresta*));
    for (int i = 0; i < numVertices; i++) {
        grafo->listaAdjacencia[i] = NULL;
    }
    return grafo;
}

double calcularDistanciaEuclidiana(Vertice* v1, Vertice* v2) {
    double soma = 0.0;
    soma += pow(v1->sepal_length - v2->sepal_length, 2);
    soma += pow(v1->sepal_width - v2->sepal_width, 2);
    soma += pow(v1->petal_length - v2->petal_length, 2);
    soma += pow(v1->petal_width - v2->petal_width, 2);
    return sqrt(soma);
}

double calcularDistanciaNormalizada(Vertice* v1, Vertice* v2) {
    double distanciaEuclidiana = calcularDistanciaEuclidiana(v1, v2);
    double distanciaMaxima = sqrt(4); 
    double distanciaNormalizada = distanciaEuclidiana / distanciaMaxima;
    return distanciaNormalizada;
}

void calcularEAdicionarArestas(Grafo* grafo) {
    for (Vertice* v1 = grafo->vertices; v1 != NULL; v1 = v1->proximo) {
        for (Vertice* v2 = v1->proximo; v2 != NULL; v2 = v2->proximo) {
            double distanciaNormalizada = calcularDistanciaNormalizada(v1, v2);
            if (distanciaNormalizada <= 0.3) {
                adicionarAresta(grafo, v1->id, v2->id);
                adicionarAresta(grafo, v2->id, v1->id);
            }
        }
    }
}

void carregarDadosIris(Grafo* grafo, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o Dataset da Iris em CSV.\n");
        exit(1);
    }

    char linha[200];
    fgets(linha, sizeof(linha), arquivo);

    int id = 1;

    while (fgets(linha, sizeof(linha), arquivo)) {
        char variety[20];
        double sepal_length, sepal_width, petal_length, petal_width;
        sscanf(linha, "%19[^,],%lf,%lf,%lf,%lf", variety, &sepal_length, &sepal_width, &petal_length, &petal_width);

        Vertice* vertice = criarVertice(id, sepal_length, sepal_width, petal_length, petal_width);
        adicionarVertice(grafo, vertice);
        id++;
    }

    fclose(arquivo);
}

void salvarGrafo(Grafo* grafo, const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        exit(1);
    }

    fprintf(arquivo, "%d\n", grafo->numVertices);

    for (int i = 0; i < grafo->numVertices; i++) {
        Aresta* arestaAtual = grafo->listaAdjacencia[i];
        while (arestaAtual != NULL) {
            fprintf(arquivo, "%d,%d\n", i + 1, arestaAtual->idAlvo);
            arestaAtual = arestaAtual->proxima;
        }
    }

    fclose(arquivo);
}

void liberarGrafo(Grafo* grafo) {
    for (int i = 0; i < grafo->numVertices; i++) {
        Aresta* arestaAtual = grafo->listaAdjacencia[i];
        while (arestaAtual != NULL) {
            Aresta* arestaTemp = arestaAtual;
            arestaAtual = arestaAtual->proxima;
            free(arestaTemp);
        }
    }

    free(grafo->listaAdjacencia);

    Vertice* verticeAtual = grafo->vertices;
    while (verticeAtual != NULL) {
        Vertice* verticeTemp = verticeAtual;
        verticeAtual = verticeAtual->proximo;
        free(verticeTemp);
    }

    free(grafo);
}

void BFS(Grafo* grafo, int start, bool* visitado, int* clusters, int clusterID) {
    int fila[grafo->numVertices];
    int frente = 0, tras = 0;
    fila[tras++] = start;
    visitado[start] = true;
    clusters[start] = clusterID;

    while (frente < tras) {
        int atual = fila[frente++];
        Aresta* adj = grafo->listaAdjacencia[atual];
        
        while (adj != NULL) {
            int vizinho = adj->idAlvo - 1;
            if (!visitado[vizinho]) {
                visitado[vizinho] = true;
                clusters[vizinho] = clusterID;
                fila[tras++] = vizinho;
            }
            adj = adj->proxima;
        }
    }
}

int identificarComponentesConexos(Grafo* grafo, int* clusters) {
    bool visitado[grafo->numVertices];
    for (int i = 0; i < grafo->numVertices; i++) {
        visitado[i] = false;
        clusters[i] = -1;
    }

    int clusterID = 0;
    for (int i = 0; i < grafo->numVertices; i++) {
        if (!visitado[i]) {
            BFS(grafo, i, visitado, clusters, clusterID);
            clusterID++;
        }
    }
    return clusterID;
}

typedef struct {
    int id;
    int tamanho;
} Componente;

void separarEmDoisAgrupamentos(Grafo* grafo, int* clusters, int numClusters) {
    Componente componentes[numClusters];
    for (int i = 0; i < numClusters; i++) {
        componentes[i].id = i;
        componentes[i].tamanho = 0;
    }

    for (int i = 0; i < grafo->numVertices; i++) {
        componentes[clusters[i]].tamanho++;
    }

    // Ordenar componentes por tamanho para escolher os dois maiores
    qsort(componentes, numClusters, sizeof(Componente), compararTamanhos);

    // Atribuir todos os vértices aos dois maiores clusters
    for (int i = 0; i < grafo->numVertices; i++) {
        if (clusters[i] != componentes[0].id && clusters[i] != componentes[1].id) {
            if (distanciaParaCluster(grafo->vertices[i], grafo, componentes[0].id) < 
                distanciaParaCluster(grafo->vertices[i], grafo, componentes[1].id)) {
                clusters[i] = componentes[0].id;
            } else {
                clusters[i] = componentes[1].id;
            }
        }
    }
}

int compararTamanhos(const void* a, const void* b) {
    return ((Componente*)b)->tamanho - ((Componente*)a)->tamanho;
}

void calcularCentro(Grafo* grafo, int* clusters, int clusterID, double* centro) {
    int count = 0;
    double somaSepalLength = 0.0, somaSepalWidth = 0.0;
    double somaPetalLength = 0.0, somaPetalWidth = 0.0;

    for (Vertice* v = grafo->vertices; v != NULL; v = v->proximo) {
        if (clusters[v->id - 1] == clusterID) {
            somaSepalLength += v->sepal_length;
            somaSepalWidth += v->sepal_width;
            somaPetalLength += v->petal_length;
            somaPetalWidth += v->petal_width;
            count++;
        }
    }

    centro[0] = somaSepalLength / count;
    centro[1] = somaSepalWidth / count;
    centro[2] = somaPetalLength / count;
    centro[3] = somaPetalWidth / count;
}

int main() {
    Grafo* grafoIris = criarGrafo(150);
    carregarDadosIris(grafoIris, "IrisDataset.csv");
    calcularEAdicionarArestas(grafoIris);

    int clusters[grafoIris->numVertices];
    int numClusters = identificarComponentesConexos(grafoIris, clusters);
    separarEmDoisAgrupamentos(grafoIris, clusters, numClusters);

    double centroSetosa[4], centroNaoSetosa[4];
    calcularCentro(grafoIris, clusters, 0, centroSetosa);    // ID 0
    calcularCentro(grafoIris, clusters, 1, centroNaoSetosa); // ID 1

    salvarGrafo(grafoIris, "iris_grafo_agrupado.txt");
    liberarGrafo(grafoIris);

    return 0;
}
