#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

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
  return sqrt(pow(v1->sepal_length - v2->sepal_length, 2) +
              pow(v1->sepal_width - v2->sepal_width, 2) +
              pow(v1->petal_length - v2->petal_length, 2) +
              pow(v1->petal_width - v2->petal_width, 2));
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

int main() {
    Grafo* grafoIris = criarGrafo(150);
    carregarDadosIris(grafoIris, "IrisDataset.csv");
    calcularEAdicionarArestas(grafoIris);
    salvarGrafo(grafoIris, "iris_grafo.txt");
    liberarGrafo(grafoIris);

    return 0;
}
