#pragma once

#include "Node.hpp"
#include "Component.hpp"
#include <vector>
#include <memory>
#include <string>

class Circuit {
public:
    Circuit();
    ~Circuit() = default;

    // --- Budowanie obwodu ---

    // Tworzy nowy węzeł i zwraca wskaźnik do niego.
    // Węzeł GND (id=0) tworzony jest automatycznie w konstruktorze.
    Node* addNode(const std::string& name = "");

    // Dodaje gotowy komponent (Circuit przejmuje własność przez unique_ptr)
    void addComponent(std::unique_ptr<Component> comp);

    // Wygodny dostęp do węzła GND
    Node* gnd() const { return nodes_[0].get(); }

    // --- Symulacja ---

    // Buduje macierz MNA i rozwiązuje układ równań.
    // Po wywołaniu: node->voltage jest wypełnione dla każdego węzła.
    // Zwraca false jeśli macierz jest osobliwa (zły obwód).
    bool solve();

    // --- Diagnostyka ---
    void printNodeVoltages() const;
    void printComponents() const;

private:
    std::vector<std::unique_ptr<Node>>      nodes_;
    std::vector<std::unique_ptr<Component>> components_;

    // Eliminacja Gaussa – rozwiązuje Ax = b, wynik w b
    // Zwraca false jeśli macierz osobliwa
    bool gaussianElimination(Matrix& A, Vector& b) const;
};