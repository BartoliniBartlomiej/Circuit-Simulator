#include "Circuit.hpp"
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
    // Liczba węzłów bez GND – to rozmiar macierzy MNA
    int n = static_cast<int>(nodes_.size()) - 1;
    if (n <= 0) {
        std::cerr << "[Circuit] Brak węzłów do symulacji.\n";
        return false;
    }

    // Inicjalizacja macierzy G i wektora I zerami
    Matrix G(n, Vector(n, 0.0));
    Vector I(n, 0.0);

    // Każdy komponent dodaje swój wkład
    for (const auto& comp : components_) {
        comp->stamp(G, I);
    }

    // Rozwiązujemy G * V = I  →  V to napięcia węzłów
    // Kopia wektora I – gaussianElimination nadpisuje go wynikiem
    Vector V = I;
    if (!gaussianElimination(G, V)) {
        std::cerr << "[Circuit] Macierz osobliwa – sprawdź obwód.\n";
        return false;
    }

    // Przepisujemy wyniki z powrotem do węzłów
    // nodes_[0] = GND (0V), nodes_[1..n] = węzły obliczone
    for (int i = 0; i < n; ++i) {
        nodes_[i + 1]->voltage = V[i];
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