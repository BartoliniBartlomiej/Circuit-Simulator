#pragma once
#include <iostream>
#include <string>

class Instrument {
protected:
    std::string name;

public:
    Instrument(const std::string& n) : name(n) {}

    virtual ~Instrument() = default;

    virtual double measure() = 0;

    virtual void showResult() {
        std::cout << name << ": " << measure() << std::endl;
    }
};

class Voltmeter : public Instrument {
public:
    Voltmeter() : Instrument("Voltmeter") {}

    double measure() override {
        return 12.5;
    }
};

class Ammeter : public Instrument {
public:
    Ammeter() : Instrument("Ammeter") {}

    double measure() override {
        return 1.8;
    }
};

class Ohmmeter : public Instrument {
public:
    Ohmmeter() : Instrument("Ohmmeter") {}

    double measure() override {
        return 220.0; 
    }
};

