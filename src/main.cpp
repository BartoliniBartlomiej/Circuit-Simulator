#include "core/Circuit.hpp"
#include "core/components/Resistor.hpp"
#include "core/components/VoltageSource.hpp"
#include "core/components/CurrentSource.hpp"
#include <iostream>

int main() {
    // Demo
    Circuit c;
    Node* vcc = c.addNode("VCC");
    Node* n1  = c.addNode("n1");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 12.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, n1,     100.0));
    c.addComponent(std::make_unique<Resistor>     ("R2", n1,  c.gnd(),200.0));

    if (c.solve()) {
        c.printNodeVoltages();
        c.printComponents();
    }

    return 0;
}