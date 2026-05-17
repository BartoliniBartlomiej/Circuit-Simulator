#include "Circuit.hpp"
#include "components/VoltageSource.hpp"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cmath>

Circuit::Circuit() {
    // Węzeł GND zawsze ma id=0 i napięcie 0V
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
    int n = static_cast<int>(nodes_.size()) - 1; // węzły bez GND
    if (n <= 0) {
        std::cerr << "[Circuit] Brak węzłów do symulacji.\n";
        return false;
    }

    // --- Zliczamy VoltageSource i przydzielamy im indeksy ---
    // Każde źródło napięcia dostaje dodatkowy wiersz/kolumnę (prąd gałęzi).
    int vsCount = 0;
    for (const auto& comp : components_) {
        if (auto* vs = dynamic_cast<VoltageSource*>(comp.get())) {
            vs->assignBranchIndex(n + vsCount);
            ++vsCount;
        }
    }

    // Rozmiar rozszerzonej macierzy: węzły + prądy źródeł napięcia
    int size = n + vsCount;

    Matrix G(size, Vector(size, 0.0));
    Vector rhs(size, 0.0);

    // Każdy komponent wbija swój wkład
    for (const auto& comp : components_) {
        comp->stamp(G, rhs);
    }

    // Rozwiązujemy rozszerzone G * x = rhs
    // x = [ V(n1), V(n2), ..., J(vs1), J(vs2), ... ]
    Vector x = rhs;
    if (!gaussianElimination(G, x)) {
        std::cerr << "[Circuit] Macierz osobliwa – sprawdź obwód.\n";
        return false;
    }

    // Przepisujemy napięcia do węzłów
    for (int i = 0; i < n; ++i) {
        nodes_[i + 1]->voltage = x[i];
    }

    // Przepisujemy prądy gałęzi do VoltageSource
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
        // Szukamy wiersza z największym elementem w kolumnie (pivoting)
        int pivotRow = col;
        double maxVal = std::abs(A[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::abs(A[row][col]) > maxVal) {
                maxVal = std::abs(A[row][col]);
                pivotRow = row;
            }
        }

        if (maxVal < EPS) return false; // macierz osobliwa

        // Zamiana wierszy
        std::swap(A[col], A[pivotRow]);
        std::swap(b[col], b[pivotRow]);

        // Eliminacja w dół
        for (int row = col + 1; row < n; ++row) {
            double factor = A[row][col] / A[col][col];
            for (int k = col; k < n; ++k) {
                A[row][k] -= factor * A[col][k];
            }
            b[row] -= factor * b[col];
        }
    }

    // Podstawianie wsteczne
    for (int row = n - 1; row >= 0; --row) {
        for (int k = row + 1; k < n; ++k) {
            b[row] -= A[row][k] * b[k];
        }
        b[row] /= A[row][row];
    }

    return true;
}

void Circuit::printNodeVoltages() const {
    std::cout << "\n=== Napięcia węzłów ===\n";
    for (const auto& node : nodes_) {
        std::cout << std::setw(8) << node->name
                  << "  →  " << std::fixed << std::setprecision(4)
                  << node->voltage << " V\n";
    }
}

void Circuit::printComponents() const {
    std::cout << "\n=== Komponenty ===\n";
    for (const auto& comp : components_) {
        std::cout << std::setw(10) << comp->name
                  << "  I = " << std::fixed << std::setprecision(4)
                  << comp->getCurrent() << " A\n";
    }
}