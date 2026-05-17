#pragma once
#include "../Component.hpp"

// Idealne źródło napięcia między nodeA (+) i nodeB (-).
//
// Wymaga rozszerzonego MNA: Circuit musi zarezerwować dla każdego
// VoltageSource dodatkowy wiersz/kolumnę na prąd gałęzi (branchCurrentIndex_).
// Circuit::solve() wywołuje assignBranchIndex() przed budową macierzy.
//
// Rozszerzona macierz (n węzłów + k źródeł napięcia):
//
//   [ G   A ] [ V ]   [ I  ]
//   [ Aᵀ  0 ] [ J ] = [ Vs ]
//
//   A[a][j] =  1,  A[b][j] = -1  (j = indeks prądu tej gałęzi)
//   Aᵀ[j][a] = 1, Aᵀ[j][b] = -1
//   rhs[n+j] = voltage_

class VoltageSource : public Component {
public:
    VoltageSource(const std::string& name, Node* nodeA, Node* nodeB, double voltage)
        : Component(name, nodeA, nodeB), voltage_(voltage), branchIndex_(-1)
    {}

    // Wywoływane przez Circuit::solve() przed stamp() –
    // przydziela indeks wiersza/kolumny dla prądu tej gałęzi.
    // idx = (liczba węzłów bez GND) + numer kolejny tego źródła
    void assignBranchIndex(int idx) { branchIndex_ = idx; }
    int  branchIndex() const        { return branchIndex_; }

    void stamp(Matrix& G, Vector& rhs) const override {
        // branchIndex_ musi być ustawiony przed stamp()
        int j = branchIndex_;

        auto idx = [](const Node* n) { return n->id - 1; };

        // Kolumna j (wpływ prądu gałęzi na węzły)
        if (!nodeA->isGround()) {
            G[idx(nodeA)][j] += 1.0;
            G[j][idx(nodeA)] += 1.0;
        }
        if (!nodeB->isGround()) {
            G[idx(nodeB)][j] -= 1.0;
            G[j][idx(nodeB)] -= 1.0;
        }

        // Równanie: V(a) - V(b) = voltage_
        rhs[j] = voltage_;
    }

    // Prąd gałęzi – przechowywany po solve() przez Circuit
    double getCurrent() const override { return branchCurrent_; }
    void   setBranchCurrent(double i)  { branchCurrent_ = i; }

    double voltage() const  { return voltage_; }
    void setVoltage(double v) { voltage_ = v; }

private:
    double voltage_;       // [V]
    int branchIndex_;   // indeks w rozszerzonej macierzy
    double branchCurrent_ = 0.0; // [A], wypełniane po solve()
    
};