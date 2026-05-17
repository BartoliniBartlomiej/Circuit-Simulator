#pragma once
#include "../Component.hpp"
#include <stdexcept>

// Rezystor między nodeA i nodeB.
//
// stamp (konduktancja g = 1/R):
//   G[a][a] += g
//   G[b][b] += g
//   G[a][b] -= g
//   G[b][a] -= g
// (wiersze/kolumny GND są pomijane)

class Resistor : public Component {
public:
    Resistor(const std::string& name, Node* nodeA, Node* nodeB, double resistance)
        : Component(name, nodeA, nodeB), resistance_(resistance)
    {
        if (resistance_ <= 0.0)
            throw std::invalid_argument("Resistance must be > 0");
    }

    void stamp(Matrix& G, Vector& /*I*/) const override {
        double g = 1.0 / resistance_;

        // Pomocnicze lambda – indeks w macierzy (0-based, bez GND)
        auto idx = [](const Node* n) { return n->id - 1; };

        bool aIsGnd = nodeA->isGround();
        bool bIsGnd = nodeB->isGround();

        if (!aIsGnd && !bIsGnd) {
            G[idx(nodeA)][idx(nodeA)] += g;
            G[idx(nodeB)][idx(nodeB)] += g;
            G[idx(nodeA)][idx(nodeB)] -= g;
            G[idx(nodeB)][idx(nodeA)] -= g;
        } else if (!aIsGnd) {
            G[idx(nodeA)][idx(nodeA)] += g;
        } else if (!bIsGnd) {
            G[idx(nodeB)][idx(nodeB)] += g;
        }
        // oba GND → rezystor zwarty do masy, brak wpływu na macierz
    }

    // Prąd dostępny po Circuit::solve()
    double getCurrent() const override {
        return (nodeA->voltage - nodeB->voltage) / resistance_;
    }

    double resistance() const { return resistance_; }
    void setResistance(double r) {
        if (r <= 0.0) throw std::invalid_argument("Resistance must be > 0");
        resistance_ = r;
    }

private:
    double resistance_; // [Ω]
};