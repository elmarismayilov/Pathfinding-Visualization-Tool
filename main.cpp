#include <SFML/Graphics.hpp>
#include <vector>
#include <climits>
#include <algorithm>

const int V = 6;
const int NODE_RADIUS = 20;
sf::CircleShape nodes[V];
sf::Vector2f positions[V] = {{100, 100}, {300, 100}, {500, 100}, {200, 300}, {400, 300}, {300, 500}};
std::vector<int> path;
int graph[V][V];
int shortest_distance[V];
int previous_node[V];
bool visited[V];
int selectedNode = -1;

void init_matrix(int matrix[V][V]) {
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            matrix[i][j] = 0;
        }
    }
}

void add_edge(int matrix[V][V], int u, int v, int distance) {
    matrix[u][v] = distance;
    matrix[v][u] = distance;
}

void test_graph(int graph[V][V]) {
    add_edge(graph, 0, 1, 2);
    add_edge(graph, 0, 3, 8);
    add_edge(graph, 1, 3, 5);
    add_edge(graph, 1, 4, 6);
    add_edge(graph, 3, 4, 3);
    add_edge(graph, 3, 5, 2);
    add_edge(graph, 4, 5, 1);
    add_edge(graph, 2, 4, 9);
    add_edge(graph, 2, 5, 3);
}

int minimal_node(bool visited[V], int shortest_distance[V]) {
    int min_distance = INT_MAX, minimal_node = -1;
    for (int i = 0; i < V; i++) {
        if (!visited[i] && shortest_distance[i] < min_distance) {
            min_distance = shortest_distance[i];
            minimal_node = i;
        }
    }
    return minimal_node;
}

void distance(int graph[V][V], int source, bool visited[V], int shortest_distance[V], int previous_node[V]) {
    for (int n = 0; n < V; n++) {
        if (!visited[n] && graph[source][n] && shortest_distance[source] + graph[source][n] < shortest_distance[n]) {
            shortest_distance[n] = shortest_distance[source] + graph[source][n];
            previous_node[n] = source;
        }
    }
    visited[source] = true;
}

std::vector<int> reconstruct_path(const int previous_node[V], int target) {
    std::vector<int> path;
    for (int at = target; at != -1; at = previous_node[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

void highlight_path(sf::RenderWindow& window) {
    for (size_t i = 1; i < path.size(); i++) {
        sf::Vertex line[] = {
            sf::Vertex(positions[path[i - 1]], sf::Color::Yellow),
            sf::Vertex(positions[path[i]], sf::Color::Yellow)
        };
        window.draw(line, 2, sf::Lines);
    }
}

void drawGraph(sf::RenderWindow& window, int graph[V][V], sf::Font& font) {
    for (int i = 0; i < V; i++) {
        nodes[i].setRadius(NODE_RADIUS);
        nodes[i].setFillColor(sf::Color::Green);
        nodes[i].setOrigin(NODE_RADIUS, NODE_RADIUS);
        nodes[i].setPosition(positions[i]);
        window.draw(nodes[i]);

        sf::Text nodeText;
        nodeText.setFont(font);
        nodeText.setString(std::to_string(i));
        nodeText.setCharacterSize(18);
        nodeText.setFillColor(sf::Color::White);
        nodeText.setStyle(sf::Text::Bold);
        nodeText.setPosition(nodes[i].getPosition().x - nodeText.getLocalBounds().width / 2, nodes[i].getPosition().y - nodeText.getLocalBounds().height / 2);
        window.draw(nodeText);
    }

    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if (graph[i][j] > 0) {
                sf::Vertex line[] = {
                    sf::Vertex(positions[i], sf::Color::White),
                    sf::Vertex(positions[j], sf::Color::White)
                };
                window.draw(line, 2, sf::Lines);

                sf::Text distanceText;
                distanceText.setFont(font);
                distanceText.setString(std::to_string(graph[i][j]));
                distanceText.setCharacterSize(20);
                distanceText.setFillColor(sf::Color::Cyan);
                sf::Vector2f midPoint = (positions[i] + positions[j]) / 2.f;
                distanceText.setPosition(midPoint.x + 10, midPoint.y - 10);
                window.draw(distanceText);
            }
        }
    }

    if (selectedNode != -1) {
        sf::Text distanceToNodeText;
        distanceToNodeText.setFont(font);
        distanceToNodeText.setString("Shortest distance from Node 0 to Node " + std::to_string(selectedNode) + ": " + std::to_string(shortest_distance[selectedNode]));
        distanceToNodeText.setCharacterSize(18);
        distanceToNodeText.setFillColor(sf::Color::White);
        distanceToNodeText.setPosition(10, window.getSize().y - 40);
        window.draw(distanceToNodeText);
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 600), "Visualized Path Find");
    sf::Font font;
    if (!font.loadFromFile("#FONT_PATH")) return -1;

    init_matrix(graph);
    test_graph(graph);

    for (int i = 0; i < V; i++) {
        shortest_distance[i] = INT_MAX;
        previous_node[i] = -1;
        visited[i] = false;
    }
    shortest_distance[0] = 0;
    for (int i = 0; i < V - 1; i++) {
        int current = minimal_node(visited, shortest_distance);
        if (current == -1) break;
        distance(graph, current, visited, shortest_distance, previous_node);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                for (int i = 0; i < V; i++) {
                    if (nodes[i].getGlobalBounds().contains(mousePos)) {
                        if (selectedNode != -1) nodes[selectedNode].setFillColor(sf::Color::Green);
                        selectedNode = i;
                        nodes[i].setFillColor(sf::Color::Blue);
                        path = reconstruct_path(previous_node, selectedNode);
                        break;
                    }
                }
            }
        }

        window.clear();
        drawGraph(window, graph, font);
        highlight_path(window);
        window.display();
    }

    return 0;
}
