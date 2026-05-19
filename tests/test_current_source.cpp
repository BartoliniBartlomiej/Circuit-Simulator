#include <gtest/gtest.h>
#include "core/Circuit.hpp"
#include "core/components/Resistor.hpp"
#include "core/components/CurrentSource.hpp"
#include "core/components/VoltageSource.hpp"

static constexpr double EPS = 1e-6;

// ----------------------------------------------------------------
//  I=2A, R=5Ω → V(n1) = 10V
// ----------------------------------------------------------------
TEST(CurrentSourceTest, BasicVoltage) {
    Circuit c;
    Node* n1 = c.addNode("n1");

    c.addComponent(std::make_unique<CurrentSource>("I1", n1,  c.gnd(), 2.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", n1,  c.gnd(), 5.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(n1->voltage, 10.0, EPS);
}

// ----------------------------------------------------------------
// 2 sources at the same node
//   I1=1A, I2=3A, R=2Ω → V(n1) = (1+3)*2 = 8V
// ----------------------------------------------------------------
TEST(CurrentSourceTest, TwoSourcesSameNode) {
    Circuit c;
    Node* n1 = c.addNode("n1");

    c.addComponent(std::make_unique<CurrentSource>("I1", n1, c.gnd(), 1.0));
    c.addComponent(std::make_unique<CurrentSource>("I2", n1, c.gnd(), 3.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", n1, c.gnd(), 2.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(n1->voltage, 8.0, EPS);
}

// ----------------------------------------------------------------
// Current source + Voltage source -> Nortona/Thevenin transform
//   V1=6V on VCC, R1=10Ω (VCC→n1), R2=20Ω (n1→GND)
//   I1=0.5A flows from GND to n1 (nodeA=n1, nodeB=GND → current flows into n1)
//   KCL w n1: (6 - V_n1)/10 + 0.5 = V_n1/20
//   6/10 - V_n1/10 + 0.5 = V_n1/20
//   1.1 = V_n1*(1/20 + 1/10) = V_n1 * 3/20
//   V_n1 = 1.1 * 20/3 ≈ 7.333V
// ----------------------------------------------------------------
TEST(CurrentSourceTest, MixedSources) {
    Circuit c;
    Node* vcc = c.addNode("VCC");
    Node* n1  = c.addNode("n1");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 6.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, n1,   10.0));
    c.addComponent(std::make_unique<Resistor>     ("R2", n1,  c.gnd(), 20.0));
    c.addComponent(std::make_unique<CurrentSource>("I1", n1,  c.gnd(), 0.5));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(n1->voltage, 22.0 / 3.0, EPS);
}

// ----------------------------------------------------------------
// 
// ----------------------------------------------------------------
TEST(CurrentSourceTest, GetCurrentReturnsSetValue) {
    Circuit c;
    Node* n1 = c.addNode("n1");
    auto* src = new CurrentSource("I1", n1, c.gnd(), 3.14);
    c.addComponent(std::unique_ptr<CurrentSource>(src));
    c.addComponent(std::make_unique<Resistor>("R1", n1, c.gnd(), 1.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(src->getCurrent(), 3.14, EPS);
}
