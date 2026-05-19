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

    // --- Building the circuit ---

    // Creates a new node and returns a pointer to it.
    // The GND node (id=0) is automatically created in the constructor.
    Node* addNode(const std::string& name = "");

    void addComponent(std::unique_ptr<Component> comp);

    Node* gnd() const { return nodes_[0].get(); }

    // --- Simulation ---

    // Builds the MNA matrix and solves the system of equations.
    // After calling: node->voltage is filled for each node.
    // Returns false if the matrix is singular (bad circuit).
    bool solve();

    // --- Diagnostics ---
    void printNodeVoltages() const;
    void printComponents() const;

private:
    std::vector<std::unique_ptr<Node>>      nodes_;
    std::vector<std::unique_ptr<Component>> components_;

    // Gaussian elimination – solves Ax = b, result in b
    // Returns false if the matrix is singular
    bool gaussianElimination(Matrix& A, Vector& b) const;
};