#pragma once

#include <string>
#include <vector>

class Node;

// Aliases
using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

class Component {
public:
    std::string name;
    Node* nodeA;  // node "+"
    Node* nodeB;  // node "-" / GND

    Component(const std::string& name, Node* a, Node* b)
        : name(name), nodeA(a), nodeB(b)
    {}

    virtual ~Component() = default;

    // Każdy komponent "wbija" swój wkład do macierzy układu równań.
    // Rozmiar macierzy: (n-1) x (n-1), gdzie n = liczba węzłów (bez GND).
    virtual void stamp(Matrix& G, Vector& I) const = 0;

    // Current flowing from nodeA to nodeB (positive if flowing from A to B)
    virtual double getCurrent() const = 0;
};