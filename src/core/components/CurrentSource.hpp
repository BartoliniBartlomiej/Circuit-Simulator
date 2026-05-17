#pragma once
#include "../Component.hpp"

// Idealne źródło prądowe.
// Konwencja: prąd płynie z nodeB do nodeA (przez zewnętrzny obwód: A → B).
//
//   nodeA ──→── obwód ──→── nodeB
//          I (wymuszony)
//
// stamp:
//   I[a] -= I_src   (prąd "wypływa" z węzła A do macierzy)
//   I[b] += I_src   (prąd "wpływa" do węzła B)

class CurrentSource : public Component {
public:
    CurrentSource(const std::string& name, Node* nodeA, Node* nodeB, double current)
        : Component(name, nodeA, nodeB), current_(current)
    {}

    void stamp(Matrix& G, Vector& I) const override {
        // Prąd płynie z nodeB do nodeA wewnątrz źródła,
        // więc do węzła A "wpływa" prąd, z B "wypływa"
        if (!nodeA->isGround()) I[nodeA->id - 1] += current_;
        if (!nodeB->isGround()) I[nodeB->id - 1] -= current_;
    }

    double getCurrent() const override { return current_; }

    double current() const { return current_; }
    void setCurrent(double i) { current_ = i; }

private:
    double current_; // [A]
};