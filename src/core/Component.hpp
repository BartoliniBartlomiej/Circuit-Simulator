#pragma once

#include <string>
#include <vector>

// Forward declaration – Component nie musi wiedzieć o szczegółach Circuit
class Node;

// Typ macierzy i wektora MNA – proste aliasy na razie
using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

class Component {
public:
    std::string name;
    Node* nodeA;  // węzeł "+"
    Node* nodeB;  // węzeł "-" / GND

    Component(const std::string& name, Node* a, Node* b)
        : name(name), nodeA(a), nodeB(b)
    {}

    virtual ~Component() = default;

    // Każdy komponent "wbija" swój wkład do macierzy układu równań.
    // Rozmiar macierzy: (n-1) x (n-1), gdzie n = liczba węzłów (bez GND).
    virtual void stamp(Matrix& G, Vector& I) const = 0;

    // Prąd przez komponent – dostępny po Circuit::solve()
    virtual double getCurrent() const = 0;
};