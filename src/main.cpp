#include "core/Circuit.hpp"
#include "core/components/Resistor.hpp"
#include "core/components/VoltageSource.hpp"
#include "core/components/CurrentSource.hpp"
#include <iostream>

// ----------------------------------------------------------------
// Test 1: Dzielnik napięcia
//   12V → R1(100Ω) → n1 → R2(200Ω) → GND
//   Oczekiwane: V(n1) = 8V, I = 40mA
// ----------------------------------------------------------------
void testVoltageDivider() {
    std::cout << "=== Test 1: Dzielnik napięcia ===\n";
    Circuit c;
    Node* n1  = c.addNode("n1");
    Node* vcc = c.addNode("VCC");

    c.addComponent(std::make_unique<VoltageSource>("V1",  vcc, c.gnd(), 12.0));
    c.addComponent(std::make_unique<Resistor>     ("R1",  vcc, n1,     100.0));
    c.addComponent(std::make_unique<Resistor>     ("R2",  n1,  c.gnd(),200.0));

    if (c.solve()) {
        c.printNodeVoltages();
        c.printComponents();
        std::cout << "(oczekiwane: VCC=12V, n1=8V, I_R1=I_R2=0.04A)\n";
    }
}

// ----------------------------------------------------------------
// Test 2: Źródło prądowe z rezystorem
//   2A → węzeł n1 → R(5Ω) → GND
//   Oczekiwane: V(n1) = 2A * 5Ω = 10V
// ----------------------------------------------------------------
void testCurrentSource() {
    std::cout << "\n=== Test 2: Źródło prądowe ===\n";
    Circuit c;
    Node* n1 = c.addNode("n1");

    c.addComponent(std::make_unique<CurrentSource>("I1", n1,  c.gnd(), 2.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", n1,  c.gnd(), 5.0));

    if (c.solve()) {
        c.printNodeVoltages();
        c.printComponents();
        std::cout << "(oczekiwane: n1=10V, I_R1=2A)\n";
    }
}

// ----------------------------------------------------------------
// Test 3: Dwa źródła napięcia – sprawdzamy rozszerzone MNA
//   V1=9V → R1(300Ω) → n1 → R2(600Ω) → GND
//                            n1 → R3(600Ω) → n2 → V2(3V) → GND
//   Oczekiwane: V(n1)=6V, V(n2)=3V
// ----------------------------------------------------------------
void testTwoVoltageSources() {
    std::cout << "\n=== Test 3: Dwa źródła napięcia ===\n";
    Circuit c;
    Node* vcc = c.addNode("VCC");
    Node* n1  = c.addNode("n1");
    Node* n2  = c.addNode("n2");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 9.0));
    c.addComponent(std::make_unique<VoltageSource>("V2", n2,  c.gnd(), 3.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, n1,  300.0));
    c.addComponent(std::make_unique<Resistor>     ("R2", n1,  c.gnd(), 600.0));
    c.addComponent(std::make_unique<Resistor>     ("R3", n1,  n2,  600.0));

    if (c.solve()) {
        c.printNodeVoltages();
        c.printComponents();
        std::cout << "(oczekiwane: VCC=9V, n1=5.25V, n2=3V)\n";
    }
}

int main() {
    testVoltageDivider();
    testCurrentSource();
    testTwoVoltageSources();
    return 0;
}