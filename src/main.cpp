#include "core/Circuit.hpp"
#include <iostream>

// Tymczasowy komponent-stub – usuniemy go gdy dodamy Resistor.hpp
// Pozwala już teraz skompilować i przetestować Circuit::solve()
class StubResistor : public Component {
    double R_;
public:
    StubResistor(const std::string& name, Node* a, Node* b, double r)
        : Component(name, a, b), R_(r) {}

    void stamp(Matrix& G, Vector& I) const override {
        double g = 1.0 / R_;
        // id=0 to GND – pomijamy jego wiersze/kolumny w macierzy
        // Indeks w macierzy = node->id - 1
        auto idx = [](Node* n) { return n->id - 1; };

        if (!nodeA->isGround() && !nodeB->isGround()) {
            G[idx(nodeA)][idx(nodeA)] += g;
            G[idx(nodeB)][idx(nodeB)] += g;
            G[idx(nodeA)][idx(nodeB)] -= g;
            G[idx(nodeB)][idx(nodeA)] -= g;
        } else if (!nodeA->isGround()) {
            G[idx(nodeA)][idx(nodeA)] += g;
        } else if (!nodeB->isGround()) {
            G[idx(nodeB)][idx(nodeB)] += g;
        }
    }

    double getCurrent() const override {
        return (nodeA->voltage - nodeB->voltage) / R_;
    }
};

// Stub źródła napięcia – wstrzykuje prąd do węzła (uproszczenie)
class StubVSource : public Component {
    double V_;
public:
    StubVSource(const std::string& name, Node* plus, Node* gnd, double v)
        : Component(name, plus, gnd), V_(v) {}

    void stamp(Matrix& G, Vector& I) const override {
        // Uproszczone – źródło napięcia przez duży rezystor (metoda Norton)
        // Prawdziwa implementacja będzie w VoltageSource.hpp z rozszerzonym MNA
        const double Rsrc = 1e-6; // bardzo mały opór wewnętrzny
        double g = 1.0 / Rsrc;
        int i = nodeA->id - 1;
        G[i][i] += g;
        I[i]    += V_ * g;
    }

    double getCurrent() const override { return 0.0; /* stub */ }
};

int main() {
    // Test: prosty dzielnik napięcia
    //
    //   V+ (12V)
    //    |
    //   [R1 = 100Ω]   ← węzeł n1
    //    |
    //   [R2 = 200Ω]
    //    |
    //   GND
    //
    // Oczekiwane: V(n1) = 12 * 200/(100+200) = 8V

    Circuit c;
    Node* n1  = c.addNode("n1");
    Node* gnd = c.gnd();

    // Tworzymy węzeł "zasilania" – do niego podłączone źródło
    Node* vcc = c.addNode("VCC");

    c.addComponent(std::make_unique<StubVSource>("V1",  vcc, gnd, 12.0));
    c.addComponent(std::make_unique<StubResistor>("R1", vcc, n1,  100.0));
    c.addComponent(std::make_unique<StubResistor>("R2", n1,  gnd, 200.0));

    if (c.solve()) {
        c.printNodeVoltages();
        c.printComponents();
        std::cout << "\n[OK] Oczekiwane V(n1) ≈ 8.0000 V\n";
    }

    return 0;
}