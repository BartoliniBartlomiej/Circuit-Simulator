#include <gtest/gtest.h>
#include "core/Circuit.hpp"
#include "core/components/Resistor.hpp"
#include "core/components/VoltageSource.hpp"

static constexpr double EPS = 1e-6;

// ----------------------------------------------------------------
// Voltage source enforces voltage at its node
// ----------------------------------------------------------------
TEST(VoltageSourceTest, EnforcesVoltage) {
    Circuit c;
    Node* vcc = c.addNode("VCC");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 5.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, c.gnd(), 100.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(vcc->voltage, 5.0, EPS);
}

// ----------------------------------------------------------------
// Branch current: V=10V, R=200Ω → I=50mA (negative sign because current flows from VCC to GND)
// ----------------------------------------------------------------
TEST(VoltageSourceTest, BranchCurrent) {
    Circuit c;
    Node* vcc = c.addNode("VCC");

    auto* vs = new VoltageSource("V1", vcc, c.gnd(), 10.0);
    c.addComponent(std::unique_ptr<VoltageSource>(vs));
    c.addComponent(std::make_unique<Resistor>("R1", vcc, c.gnd(), 200.0));

    ASSERT_TRUE(c.solve());
    // Current flows from VCC through R to GND – MNA returns it with a negative sign
    EXPECT_NEAR(std::abs(vs->getCurrent()), 0.05, EPS);
}

// ----------------------------------------------------------------
// Two voltage sources in one circuit – extended MNA
//   V1=9V, V2=3V, R1=300Ω (VCC→n1), R2=600Ω (n1→GND), R3=600Ω (n1→n2)
//   V(n1) = 5.25V  (solution to the system of equations)
// ----------------------------------------------------------------
TEST(VoltageSourceTest, TwoVoltageSources) {
    Circuit c;
    Node* vcc = c.addNode("VCC");
    Node* n1  = c.addNode("n1");
    Node* n2  = c.addNode("n2");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 9.0));
    c.addComponent(std::make_unique<VoltageSource>("V2", n2,  c.gnd(), 3.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, n1,  300.0));
    c.addComponent(std::make_unique<Resistor>     ("R2", n1,  c.gnd(), 600.0));
    c.addComponent(std::make_unique<Resistor>     ("R3", n1,  n2,  600.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(vcc->voltage, 9.0,   EPS);
    EXPECT_NEAR(n2->voltage,  3.0,   EPS);
    EXPECT_NEAR(n1->voltage,  5.25,  EPS);
}

// ----------------------------------------------------------------
// Circuit without a path to GND – singular matrix, solve() = false
// (two floating nodes, not connected to GND)
// ----------------------------------------------------------------
TEST(VoltageSourceTest, SingularCircuitReturnsFalse) {
    Circuit c;
    Node* n1 = c.addNode("n1");
    Node* n2 = c.addNode("n2");

    // Resistor between n1 and n2, but no node is connected to GND
    c.addComponent(std::make_unique<Resistor>("R1", n1, n2, 100.0));

    EXPECT_FALSE(c.solve());
}
