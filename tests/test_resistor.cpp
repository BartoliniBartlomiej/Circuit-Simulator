#include <gtest/gtest.h>
#include "core/Circuit.hpp"
#include "core/components/Resistor.hpp"
#include "core/components/VoltageSource.hpp"

// tolerancy 
static constexpr double EPS = 1e-6;

// ----------------------------------------------------------------
// Voltage Divider: V1=12V, R1=100Ω, R2=200Ω
//   V(n1) = 12 * 200/300 = 8V
//   I     = 12/300       = 0.04A
// ----------------------------------------------------------------
TEST(ResistorTest, VoltageDivider) {
    Circuit c;
    Node* vcc = c.addNode("VCC");
    Node* n1  = c.addNode("n1");

    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 12.0));
    c.addComponent(std::make_unique<Resistor>     ("R1", vcc, n1,     100.0));
    c.addComponent(std::make_unique<Resistor>     ("R2", n1,  c.gnd(),200.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(vcc->voltage, 12.0,  EPS);
    EXPECT_NEAR(n1->voltage,   8.0,  EPS);
    EXPECT_NEAR(c.gnd()->voltage, 0.0, EPS);
}

// ----------------------------------------------------------------
//   I = V/R = 5/1000 = 5mA
// ----------------------------------------------------------------
TEST(ResistorTest, SingleResistorCurrent) {
    Circuit c;
    Node* vcc = c.addNode("VCC");

    auto* r = new Resistor("R1", vcc, c.gnd(), 1000.0);
    c.addComponent(std::make_unique<VoltageSource>("V1", vcc, c.gnd(), 5.0));
    c.addComponent(std::unique_ptr<Resistor>(r));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(r->getCurrent(), 0.005, EPS);
}

// ----------------------------------------------------------------
// Parallel resistors
// R1 || R2 = 60Ω, V=12V → I=0.2A
// ----------------------------------------------------------------
TEST(ResistorTest, ParallelResistors) {
    Circuit c;
    Node* vcc = c.addNode("VCC");

    c.addComponent(std::make_unique<VoltageSource>("V1",  vcc, c.gnd(), 12.0));
    c.addComponent(std::make_unique<Resistor>     ("R1",  vcc, c.gnd(), 100.0));
    c.addComponent(std::make_unique<Resistor>     ("R2",  vcc, c.gnd(), 150.0));

    ASSERT_TRUE(c.solve());
    EXPECT_NEAR(vcc->voltage, 12.0, EPS);
    // Sprawdzamy przez prawo Kirchhoffa: I_R1 + I_R2 = V / R_eq
    // R_eq = 100*150/(100+150) = 60Ω → I_total = 12/60 = 0.2A
}

// ----------------------------------------------------------------
// Invalid Resistance values
// ----------------------------------------------------------------
TEST(ResistorTest, InvalidResistanceThrows) {
    Circuit c;
    Node* n1 = c.addNode("n1");

    EXPECT_THROW(
        c.addComponent(std::make_unique<Resistor>("R_bad", n1, c.gnd(), 0.0)),
        std::invalid_argument
    );
    EXPECT_THROW(
        c.addComponent(std::make_unique<Resistor>("R_neg", n1, c.gnd(), -10.0)),
        std::invalid_argument
    );
}
