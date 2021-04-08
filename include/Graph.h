#pragma once
#include <functional>
#include <vector>
#include <stack>
#include "Utils.h"

namespace Starsurge {
    template <typename T, bool directed>
    class Graph;

    template <typename T>
    class GraphNode {
    public:
        GraphNode(T data) {
            this->Data = data;
        }

        bool IsIsolated() {
            return (this->edges.size() == 0);
        }
        unsigned int Degree() {
            return this->edges.size();
        }
        GraphNode<T> * Neighbour(size_t i) {
            if (i < 0 || i >= this->edges.size())
                return NULL;
            return this->edges[i];
        }
        bool IsNeighbour(GraphNode<T> * other) {
            return ElemOf<GraphNode<T>*>(this->edges, other);
        }
        bool HasLoop() {
            return IsNeighbour(this);
        }
        T Data;
    private:
        std::vector<GraphNode<T>*> edges;
        std::vector<float> weights;

        void AddEdge(GraphNode<T> * to, float weight) {
            if (to == NULL)
                return;
            int index = Index<GraphNode<T>*>(this->edges, to);
            if (index >= 0) {
                this->weights[index] = weight;
            }
            else {
                this->edges.push_back(to);
                this->weights.push_back(weight);
            }
        }

        void RemoveEdge(GraphNode<T> * to) {
            if (to == NULL || !IsNeighbour(to))
                return;
            int index = Index<GraphNode<T>*>(this->edges, to);
            if (index < 0)
                return;
            this->edges.erase(this->edges.begin()+index);
            this->weights.erase(this->weights.begin()+index);
        }

        template <typename T, bool directed>
        friend class Graph;
    };

    template <typename T, bool directed = false>
    class Graph {
    public:
        Graph() {
        }
        Graph(const Graph<T,directed>& copy) {
            std::map<GraphNode<T>*,GraphNode<T>*> translation;
            // Add the nodes first.
            for (unsigned int i = 0; i < copy.nodes.size(); ++i) {
                translation[copy.nodes[i]] = Add(copy.nodes[i]->Data);
            }

            // Add the edges.
            for (unsigned int i = 0; i < copy.nodes.size(); ++i) {
                for (unsigned int j = 0; j < copy.nodes[i]->edges.size(); ++j) {
                    Connect(translation[copy.nodes[i]], translation[copy.nodes[i]->edges[j]], copy.nodes[i]->weights[j]);
                }
            }
        }
        ~Graph() {
            while (this->nodes.size() > 0)
                Remove(this->nodes[0]);
        }

        bool Contains(GraphNode<T> * node) {
            if (node == NULL)
                return false;
            return ElemOf<GraphNode<T>*>(this->nodes, node);
        }

        GraphNode<T> * Add(T data) {
            GraphNode<T> * newNode = new GraphNode(data);
            this->nodes.push_back(newNode);
            return newNode;
        }
        void Remove(GraphNode<T> * node) {
            if (!Contains(node))
                return;
            int index = -1;
            // Find all nodes adjacent to node and remove the connection.
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                if (this->nodes[i] == node)
                    index = i;
                else
                    Disconnect(this->nodes[i], node);
            }
            // Remove node from this->nodes.
            if (index >= 0)
                this->nodes.erase(this->nodes.begin()+index);

            // Delete node
            delete node;
        }
        void Connect(GraphNode<T> * a, GraphNode<T> * b, float weight = 1) {
            if (!Contains(a) || !Contains(b))
                return;
            a->AddEdge(b, weight);
            if (!directed)
                b->AddEdge(a, weight);
        }
        void Disconnect(GraphNode<T> * a, GraphNode<T> * b) {
            if (!Contains(a) || !Contains(b))
                return;
            a->RemoveEdge(b);
            if (!directed)
                b->RemoveEdge(a);
        }

        void Transpose() {
            if (!directed)
                return;

            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                for (unsigned int j = 0; j < this->nodes[i]->edges.size(); ++j) {
                    Connect(this->nodes[i]->edges[j], this->nodes[i]);
                    Disconnect(this->nodes[i], this->nodes[i]->edges[j]);
                }
            }
        }
        std::vector<GraphNode<T>*> Search(T data) {
            std::vector<GraphNode<T*>> matches;
            for (unsigned int i = 0; this->nodes.size(); ++i) {
                if (this->nodes[i]->Data == data) {
                    matches.push_back(this->nodes[i]);
                }
            }
            return matches;
        }
        bool IsConnected() {
            return (GetConnectedComponents().size() <= 1);
        }
        bool IsConnected(GraphNode<T> * a, GraphNode<T> * b) {
            return (ShortestPath(a, b).size() > 0);
        }
        template <typename = std::enable_if<directed==false>>
        Graph<T,false> * GetMinimumSpanningTree() {
            // Prim's Algorithm.
            // Step 1. Associate a minimal cost to each vertex.
            std::map<GraphNode<T>*,float> cost;
            std::map<GraphNode<T>*,std::tuple<GraphNode<T>*,GraphNode<T>*,float>> edge;
            std::map<GraphNode<T>*,GraphNode<T>*> translation;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                cost[this->nodes[i]] = Infinity();
                edge[this->nodes[i]] = std::make_tuple((GraphNode<T>*)NULL, (GraphNode<T>*)NULL, 0.0f);
                translation[this->nodes[i]] = NULL;
            }

            // Step 2. Start a forest.
            Graph<T, false> * tree = new Graph();
            std::vector<GraphNode<T>*> q = this->nodes;

            // Step 3. Repeat until q is empty.
            while (q.size() > 0) {
                // Step 3a. Find and remove the vertex from q with lowest cost.
                float min = Infinity();
                GraphNode<T> * v = NULL;
                unsigned int index = 0;
                for (unsigned int i = 0; i < q.size(); ++i) {
                    if (cost[q[i]] <= min) {
                        min = cost[q[i]];
                        v = q[i];
                        index = i;
                    }
                }
                q.erase(q.begin()+index);

                // Step 3b. Add v and edge[v] (if not NULL) to tree
                if (translation[v] == NULL)
                    translation[v] = tree->Add(v->Data);
                if (std::get<0>(edge[v]) != NULL && std::get<1>(edge[v]) != NULL) {
                    if (translation[std::get<0>(edge[v])] == NULL) {
                        translation[std::get<0>(edge[v])] = tree->Add(std::get<0>(edge[v])->Data);
                    }
                    if (translation[std::get<1>(edge[v])] == NULL) {
                        translation[std::get<1>(edge[v])] = tree->Add(std::get<1>(edge[v])->Data);
                    }
                    tree->Connect(translation[std::get<0>(edge[v])], translation[std::get<1>(edge[v])], std::get<2>(edge[v]));
                }

                // Step 3c. For all edges (v,w), if w is in q and it's weight is lower than cost[w], then set cost[w] and edge[w].
                for (unsigned int i = 0; i < v->edges.size(); ++i) {
                    if (ElemOf<GraphNode<T>*>(q, v->edges[i])) {
                        if (v->weights[i] < cost[v->edges[i]]) {
                            cost[v->edges[i]] = v->weights[i];
                            edge[v->edges[i]] = std::make_tuple(v, v->edges[i], v->weights[i]);
                        }
                    }
                }
            }

            return tree;
        }
        std::vector<Graph<T,directed>*> GetConnectedComponents() {
            std::vector<Graph<T,directed>*> components;
            std::map<GraphNode<T>*,GraphNode<T>*> translation;

            // Initialize the depth and lowlink with -1 and onStack with false.
            std::map<GraphNode<T>*,int> depth;
            std::map<GraphNode<T>*,int> lowlink;
            std::map<GraphNode<T>*,bool> onStack;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                depth[this->nodes[i]] = -1;
                lowlink[this->nodes[i]] = -1;
                onStack[this->nodes[i]] = false;
            }

            // Perform Tarjan (https://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm)
            unsigned int index = 0;
            std::stack<GraphNode<T>*> stack;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                if (depth[this->nodes[i]] == -1) {
                    std::vector<Graph<T,directed>*> ret = StrongConnect(this->nodes[i], index, stack, depth, lowlink, onStack, translation);
                    for (unsigned int j = 0; j < ret.size(); ++j) {
                        components.push_back(ret[j]);
                    }
                }
            }

            // Add the edges to our components.
            for (unsigned int i = 0; i < components.size(); ++i) {
                for (unsigned int j = 0; j < components[i]->nodes.size(); ++j) {
                    GraphNode<T> * v = components[i]->nodes[j];
                    for (unsigned int k = 0; k < translation[v]->edges.size(); ++k) {
                        // Search for translation[v]->edges in translation.
                        for (auto it = translation.begin(); it != translation.end(); ++it) {
                            if (it->second == translation[v]->edges[k]) {
                                components[i]->Connect(v, it->first);
                                break;
                            }
                        }
                    }
                }
            }

            return components;
        }
        std::vector<GraphNode<T>*> GetShortestPath(GraphNode<T> * from, GraphNode<T> * to) {
            // Following Dijkstra's algorithm
            if (from == NULL || to == NULL)
                return std::vector<GraphNode<T>*>();
            std::vector<GraphNode<T>*> unvisited;
            std::map<GraphNode<T>*,float> distance;
            std::map<GraphNode<T>*,GraphNode<T>*> previous;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                unvisited.push_back(this->nodes[i]);
                distance[this->nodes[i]] = Infinity();
                previous[this->nodes[i]] = NULL;
            }
            distance[from] = 0;

            while (unvisited.size() > 0) {
                // Get the node from unvisited with the smallest tentative distance
                float min = Infinity();
                GraphNode<T> * cur = NULL;
                unsigned int index = 0;
                for (unsigned int i = 0; i < unvisited.size(); ++i) {
                    if (distance[unvisited[i]] <= min) {
                        min = distance[unvisited[i]];
                        cur = unvisited[i];
                        index = i;
                    }
                }
                unvisited.erase(unvisited.begin()+index);

                // Are we there yet?
                if (cur == to)
                    break;

                // Find the neighbour with lowest distance.
                for (unsigned int i = 0; i < cur->edges.size(); ++i) {
                    float alt = distance[cur] + cur->weights[i];
                    if (alt < distance[cur->edges[i]]) {
                        distance[cur->edges[i]] = alt;
                        previous[cur->edges[i]] = cur;
                    }
                }
            }

            // Now we walk backwards for the path.
            std::vector<GraphNode<T>*> path;
            GraphNode<T> * cur = to;
            if (previous[cur] != NULL || cur == from) {
                while (cur != NULL) {
                    path.insert(path.begin(), cur);
                    cur = previous[cur];
                }
            }
            return path;
        }

        std::string ToDOT() {
            return ToDOT([](T t) { return ToString(t); });
        }

        std::string ToDOT(std::function<std::string(T)> print) {
            std::string dot = "";
            if (directed) dot += "digraph g {\n";
            else dot += "graph g {\n";

            std::string arrow = "--";
            if (directed) arrow = "->";
            std::vector<std::pair<GraphNode<T>*,GraphNode<T>*>> edges;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                dot += "\tnode"+ToString((int)this->nodes[i])+" [label=\""+print(this->nodes[i]->Data)+"\"];\n";
                for (unsigned int j = 0; j < this->nodes[i]->edges.size(); ++j) {
                    if (!directed) {
                        if (ElemOf<std::pair<GraphNode<T>*,GraphNode<T>*>>(edges, std::make_pair(this->nodes[i]->edges[j], this->nodes[i])))
                            continue;
                        edges.push_back(std::make_pair(this->nodes[i], this->nodes[i]->edges[j]));
                    }
                    dot += "\tnode"+ToString((int)this->nodes[i])+" "+arrow+" node"+ToString((int)this->nodes[i]->edges[j]);
                    dot += " [label=\""+ToString(this->nodes[i]->weights[j])+"\"];\n";
                }
            }

            dot += "}";
            return dot;
        }

        bool HasCycle() const {
            std::map<GraphNode<T>*,bool> visited;
            std::map<GraphNode<T>*,bool> stack;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                visited[this->nodes[i]] = false;
                stack[this->nodes[i]] = false;
            }

            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                if (_HasCycle(this->nodes[i], visited, stack)) {
                    return true;
                }
            }
            return false;
        }
        bool IsRegular() const {
            if (IsEmpty())
                return true;
            unsigned int k = this->nodes[0]->Degree();
            for (unsigned int i = 1; i < this->nodes.size(); ++i) {
                if (this->nodes[i].Degree() != k) {
                    return false;
                }
            }
            return true;
        }
        bool IsComplete() const {
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                if (this->nodes[i].Degree() != this->nodes.size()-1) {
                    return false;
                }
            }
            return true;
        }
        GraphNode<T> * Get(unsigned int i) const {
            if (i < 0 || i >= this->nodes.size())
                return NULL;
            return this->nodes[i];
        }
        unsigned int Order() const {
            return this->nodes.size();
        }
        unsigned int NumEdges() const {
            unsigned int count = 0;
            for (unsigned int i = 0; i < this->nodes.size(); ++i) {
                count += this->nodes[i]->Degree();
            }
            if (!directed)
                count /= 2;
            return count;
        }
        unsigned int Size() const {
            return Order() + NumEdges();
        }
        bool IsEmpty() const {
            return (this->nodes.size() == 0);
        }
    private:
        std::vector<GraphNode<T>*> nodes;

        std::vector<Graph<T,directed>*> StrongConnect(GraphNode<T> * v, unsigned int & index, std::stack<GraphNode<T>*> & stack, std::map<GraphNode<T>*,int> & depth, std::map<GraphNode<T>*,int> & lowlink, std::map<GraphNode<T>*,bool> & onStack, std::map<GraphNode<T>*,GraphNode<T>*> & translation) {
            std::vector<Graph<T,directed>*> components;

            // Set the depth.
            depth[v] = index;
            lowlink[v] = index;
            index += 1;
            stack.push(v);
            onStack[v] = true;

            for (unsigned int i = 0; i < v->edges.size(); ++i) {
                if (depth[v->edges[i]] == -1) {
                    std::vector<Graph<T,directed>*> ret = StrongConnect(v->edges[i], index, stack, depth, lowlink, onStack, translation);
                    for (unsigned int j = 0; j < ret.size(); ++j) {
                        components.push_back(ret[j]);
                    }
                    lowlink[v] = Min(lowlink[v], lowlink[v->edges[i]]);
                }
                else if (onStack[v->edges[i]]) {
                    lowlink[v] = Min(lowlink[v], depth[v->edges[i]]);
                }
            }

            if (lowlink[v] == depth[v]) {
                Graph<T,directed> * component = new Graph();
                while (true) {
                    GraphNode<T> * w = stack.top();
                    stack.pop();
                    onStack[w] = false;
                    GraphNode<T> * newnode = component->Add(w->Data);
                    translation[newnode] = w;
                    if (w == v)
                        break;
                }
                components.push_back(component);
            }
            return components;
        }
        bool _HasCycle(GraphNode<T> * v, std::map<GraphNode<T>*,bool> & visited, std::map<GraphNode<T>*,bool> & stack) const {
            if (visited[v] == false) {
                visited[v] = true;
                stack[v] = true;

                for (unsigned int i = 0; i < v->edges.size(); ++i) {
                    if (!visited[v->edges[i]] && _HasCycle(v->edges[i], visited, stack))
                        return true;
                    else if (stack[v->edges[i]])
                        return true;
                }
            }
            stack[v] = false;
            return false;
        }
    };

    template <typename T>
    using Digraph = Graph<T, true>;
}
