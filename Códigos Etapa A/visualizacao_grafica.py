import networkx as nx
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def visualizar_grafo_3d(arquivo):
    # Lê o arquivo e cria o grafo
    with open(arquivo, "r") as file:
        lines = file.readlines()
    
    G = nx.Graph()
    num_vertices = int(lines[0].strip())
    
    for i in range(1, num_vertices + 1):
        G.add_node(i)
    
    for line in lines[num_vertices + 1:]:
        source, target = map(int, line.strip().split(","))
        G.add_edge(source, target)
    
    # Define a disposição dos nós em 3D
    pos = nx.spring_layout(G, dim=3)
    
    # Configura a figura para visualização em 3D
    fig = plt.figure(figsize=(10, 8))
    ax = fig.add_subplot(111, projection="3d")
    
    # Plota as arestas
    for edge in G.edges():
        x = [pos[edge[0]][0], pos[edge[1]][0]]
        y = [pos[edge[0]][1], pos[edge[1]][1]]
        z = [pos[edge[0]][2], pos[edge[1]][2]]
        ax.plot(x, y, z, color="gray")
    
    # Plota os nós
    x_nodes = [pos[node][0] for node in G.nodes()]
    y_nodes = [pos[node][1] for node in G.nodes()]
    z_nodes = [pos[node][2] for node in G.nodes()]
    ax.scatter(x_nodes, y_nodes, z_nodes, s=50, color="skyblue", depthshade=True)
    
    # Adiciona rótulos aos nós
    for node, (x, y, z) in pos.items():
        ax.text(x, y, z, s=str(node), fontsize=10, color="black")
    
    plt.title("Visualização 3D do Grafo")
    plt.show()

# Exemplo de uso com um arquivo chamado 'iris_grafo.txt'
visualizar_grafo_3d("iris_grafo.txt")