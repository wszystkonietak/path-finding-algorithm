#include <iostream>
#include <queue>
#include <map>
#include <unordered_map>
#include <cmath>
#include <SFML/Graphics.hpp>

#define INF 9999
#define WIDTH 40
#define HEIGHT 30
 
using namespace std;

class Node {
public:
    Node () {
        x = 0;
        y = 0;
        h = 0;
        g = 0;
    };
    Node (int pos_x, int pos_y) {
        x = pos_x;
        y = pos_y;
    };
    Node (int pos_x, int pos_y, float g_) {
        x = pos_x;
        y = pos_y;
        g = g_;
    };

    Node* parent = NULL;
    float h = 0;
    int g = 0;
    int x;
    int y;

    vector<Node> find_neighbors();
    bool operator==(const Node& other) const
    { 
        return (x == other.x && y == other.y);
    };
    string str() const {
        return string("(") + std::to_string(this->y) + ", " + std::to_string(this->x) + ", g=" + std::to_string(this->g)+ ", h=" + std::to_string(this->h) +")";
    };
};

struct KeyFuncs
{
    size_t operator()(const Node& k)const
    {
        return std::hash<int>()(10003*k.x + 1*k.y);
    }

    bool operator()(const Node& a, const Node& b)const
    {
            return a.x == b.x && a.y == b.y;
    }
};

class NodeComparatorAstar {
public:
    //compare distance from start + distance to end from both nodes ant return information which one is closer
    bool operator() (const Node& a, const Node& b) const
    {
        return  a.h+a.g > b.h+b.g;
    }
};

class NodeComparatorDijkstra {
public:
    //compare two distances form start node and return information which one is closer
    bool operator() (const Node& a, const Node& b) const
    {
        return  a.g > b.g;
    }
};

class Dijkstra {
    public:
    Dijkstra () {
        start = Node(8, 4);
        end = Node(WIDTH - 10, HEIGHT - 8);
        node_size = 15;
        break_beetween_nodes = 1;
        all_node_size = node_size + break_beetween_nodes;
        SCR_WIDTH = WIDTH * (node_size + break_beetween_nodes);
        SCR_HEIGHT = HEIGHT * (node_size + break_beetween_nodes);
        window.create(sf::VideoMode(SCR_WIDTH, SCR_HEIGHT), "DIJKSTRA");
        node.setSize(sf::Vector2f(node_size, node_size));

        blockades_finished = false;
    }
    //variables used to calculate 
    Node start;
    Node end;
    vector<Node> result;
    unordered_map<Node, Node, KeyFuncs> paths;
    unordered_map<Node, int, KeyFuncs> visited;
    priority_queue<Node, vector<Node>, NodeComparatorAstar> open;
    vector<Node> blockades;
    bool blockades_finished;


    //variables used for printing
    int node_size;
    int break_beetween_nodes;
    int all_node_size;
    int SCR_WIDTH;
    int SCR_HEIGHT;
    sf::RenderWindow window;
    sf::RectangleShape node;

    void find_path();
    void count_result();
    void find_parent(Node n);
    void print_data();

    void ask_for_blockades();
    void draw_dijkstra();
    void process_input();
};

int main()
{
    Dijkstra ds;
    ds.draw_dijkstra();

    return 0;
}

void Dijkstra::find_path() {
    //push start point to the queue
    open.push(start);
    //push first value to paths 
    paths[start] = Node(INF, INF);
    bool founded = 0;
    while (founded == 0 && !open.empty()) {
        //take node for the top
        Node node = open.top();
        //find every neighbour of that node
        vector<Node> neighbours = node.find_neighbors();
        //push data to queue
        for(int i = 0; i < neighbours.size(); i++) {
            //check if neighbour was previoustly checked
            if (!paths.contains(neighbours[i])) {
                //calculate distance from the end point only used in A*
                neighbours[i].h = sqrt((neighbours[i].x-end.x)*(neighbours[i].x-end.x) + (neighbours[i].y-end.y)*(neighbours[i].y-end.y));
                //push neighbour to the queue
                open.push(neighbours[i]);
                //push the neighbour to paths it will be used to store shortest path 
                paths[neighbours[i]] = node;
            }
        }
        visited[node] = 1;   
        //check if this node isn't final
        if(node.x == end.x && node.y == end.y) {
            founded = 1;
        }
        //removing element we checked in this iteration 
        open.pop();
    }
    //count the way for start node to end
    count_result();
}

vector<Node> Node::find_neighbors() {
    std::vector<Node> result;
    //left node
    if(x > 0) {
            result.push_back(Node(x - 1, y, g+1));
    }
    //right node
    if(x < WIDTH - 1) {
            result.push_back(Node(x + 1, y, g+1));
    }
    //top node
    if(y > 0) {
            result.push_back(Node(x, y - 1, g+1));
    }
    //bottom node
    if(y < HEIGHT - 1) {
            result.push_back(Node(x, y + 1, g+1));
    }
    return result;
}

void Dijkstra::count_result() {
    result.push_back(end);
    find_parent(end);
}

void Dijkstra::find_parent(Node n) {
    if (auto search = paths.find(n); search != paths.end()) {
        result.push_back(search->second);
        find_parent(search->second);
    }
}

void Dijkstra::draw_dijkstra() {
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        process_input();

        window.clear();

        //draw background
        for (int i = 0; i < HEIGHT; i++) {
            for(int j = 0; j < WIDTH; j++) {
                node.setPosition(j * all_node_size + break_beetween_nodes, i * all_node_size + break_beetween_nodes);
                node.setFillColor(sf::Color(255, 255, 255));
                window.draw(node);
            }
        }

        //draw visited nodes
        for (const auto& [key, value] : visited) {
            node.setPosition(key.x * all_node_size + break_beetween_nodes, key.y * all_node_size + break_beetween_nodes);
            node.setFillColor(sf::Color(108, 159, 206));
            window.draw(node);
        }

        //draw shortest path
        for (int i = 0; i < result.size(); i++) {
            node.setPosition(result[i].x * all_node_size + break_beetween_nodes, result[i].y * all_node_size + break_beetween_nodes);
            node.setFillColor(sf::Color(255, 192, 0));
            window.draw(node);
        }

        //draw blockades
        for (int i = 0; i < blockades.size(); i++) {
            node.setPosition(blockades[i].x * all_node_size + break_beetween_nodes, blockades[i].y * all_node_size + break_beetween_nodes);
            node.setFillColor(sf::Color(0, 20, 0));
            window.draw(node);
        }

        //draw start point
        node.setPosition(start.x * all_node_size + break_beetween_nodes, start.y * all_node_size + break_beetween_nodes);
        node.setFillColor(sf::Color(10, 180, 30));
        window.draw(node);

        //draw end point
        node.setPosition(end.x * all_node_size + break_beetween_nodes, end.y * all_node_size + break_beetween_nodes);
        node.setFillColor(sf::Color(200, 10, 30));
        window.draw(node);

        window.display();
    }
}

void Dijkstra::ask_for_blockades() {
    //take mouse position in every moment
    sf::Vector2i position = sf::Mouse::getPosition(window);
    //if mouse position isn't over the grid finish process
    if ((position.x>WIDTH * all_node_size) || (position.y>HEIGHT * all_node_size)) return;
    //scale mouse position to grid size
    position.x = position.x / all_node_size;
    position.y = position.y / all_node_size;
    //push this position to blockades
    blockades.push_back(Node(position.x, position.y));
    //and to the paths - this will be checked when pushing neighbours
    paths.insert({Node(position.x, position.y), Node(position.x, position.y)});
}

void Dijkstra::process_input() {
    if(blockades_finished == false) {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            ask_for_blockades();
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            blockades_finished = true;
            find_path();
        }
    }
    //close window
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
    }
}

void Dijkstra::print_data() {
    size_t size = open.size();
    std::cout<<size<<std::endl;
    cout<<"___OPEN:"<<"\n";
    priority_queue<Node, vector<Node>, NodeComparatorDijkstra> tmp;
    for(int i = 0; i < size; ++i) {
       Node n =  open.top();
       tmp.push(n);
       cout << i<<" "<< n.str() << "\n";
       open.pop();
    }
    for(int i = 0; i < size; ++i) {
       Node n =  tmp.top();
       open.push(n);
       tmp.pop();
    }       
    
    std::cout<<"PATHS: "<<std::endl;
    for (const auto& [key, value] : paths)
        std::cout << "node: " << key.str() << "  parent: " << (&value!= NULL ? value.str() : "NULL") <<std::endl;
    std::cout<<"VISITED: "<<std::endl;
    for (const auto& [key, value] : visited)
        std::cout << "node: " << key.str() << " length_of_path: " << value <<std::endl;
    cout<<"------\n";
}