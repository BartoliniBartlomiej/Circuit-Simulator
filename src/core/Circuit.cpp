#include "Circuit.hpp"
#include "components/VoltageSource.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

Circuit::Circuit() {
    // Node GND always has id=0 and 0V
    nodes_.push_back(std::make_unique<Node>(0, "GND"));
}

Node* Circuit::addNode(const std::string& name) {
    int id = static_cast<int>(nodes_.size());
    nodes_.push_back(std::make_unique<Node>(id, name));
    return nodes_.back().get();
}

void Circuit::addComponent(std::unique_ptr<Component> comp) {
    components_.push_back(std::move(comp));
}

bool Circuit::solve() {
    int n = static_cast<int>(nodes_.size()) - 1; // No-GND nodes
    if (n <= 0) {
        std::cerr << "[Circuit] No nodes to simulate.\n";
        return false;
    }

    int vsCount = 0;
    for (const auto& comp : components_) {
        if (auto* vs = dynamic_cast<VoltageSource*>(comp.get())) {
            vs->assignBranchIndex(n + vsCount);
            ++vsCount;
        }
    }

    int size = n + vsCount;

    Matrix G(size, Vector(size, 0.0));
    Vector rhs(size, 0.0);

    for (const auto& comp : components_) {
        comp->stamp(G, rhs);
    }

    // x = [ V(n1), V(n2), ..., J(vs1), J(vs2), ... ]
    Vector x = rhs;
    if (!gaussianElimination(G, x)) {
        std::cerr << "[Circuit] Check circuit - singular matrix.\n";
        return false;
    }

    // Voltage in nodes
    for (int i = 0; i < n; ++i) {
        nodes_[i + 1]->voltage = x[i];
    }

    // Node current through voltage sources
    for (const auto& comp : components_) {
        if (auto* vs = dynamic_cast<VoltageSource*>(comp.get())) {
            vs->setBranchCurrent(x[vs->branchIndex()]);
        }
    }

    return true;
}

bool Circuit::gaussianElimination(Matrix& A, Vector& b) const {
    int n = static_cast<int>(b.size());
    const double EPS = 1e-12;

    for (int col = 0; col < n; ++col) {
        int pivotRow = col;
        double maxVal = std::abs(A[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::abs(A[row][col]) > maxVal) {
                maxVal = std::abs(A[row][col]);
                pivotRow = row;
            }
        }

        if (maxVal < EPS) return false; // singular matrix

        std::swap(A[col], A[pivotRow]);
        std::swap(b[col], b[pivotRow]);

        for (int row = col + 1; row < n; ++row) {
            double factor = A[row][col] / A[col][col];
            for (int k = col; k < n; ++k) {
                A[row][k] -= factor * A[col][k];
            }
            b[row] -= factor * b[col];
        }
    }

    for (int row = n - 1; row >= 0; --row) {
        for (int k = row + 1; k < n; ++k) {
            b[row] -= A[row][k] * b[k];
        }
        b[row] /= A[row][row];
    }

    return true;
}

void Circuit::printNodeVoltages() const {
    std::cout << "\n=== Nodes voltages ===\n";
    for (const auto& node : nodes_) {
        std::cout << std::setw(8) << node->name
                  << "  →  " << std::fixed << std::setprecision(4)
                  << node->voltage << " V\n";
    }
}

void Circuit::printComponents() const {
    std::cout << "\n=== Components ===\n";
    for (const auto& comp : components_) {
        std::cout << std::setw(10) << comp->name
                  << "  I = " << std::fixed << std::setprecision(4)
                  << comp->getCurrent() << " A\n";
    }
}