#include <iostream>
#include <cmath>
#include <vector>

using std::string;
using std::pair;
using std::vector;

const double EPSILON = 10e-7;

class Component {
    string name;
public:
    Component(const string &name) : name(name) {}

    void setName(const string &name) {
        Component::name = name;
    }

    const string &getName() const {
        return name;
    }
};

class Source: public Component {
    double internalResistance;
    bool orientedTowardsSecondNode;
    double value;
public:
    explicit Source(const string &name, double value = 5, bool orientedTowardsSecondNode = true) :
        Component(name), value(value), orientedTowardsSecondNode(orientedTowardsSecondNode) {}

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        internalResistance = internalResistance;
    }

    bool isOrientedTowardsSecondNode() const {
        return orientedTowardsSecondNode;
    }

    void setOrientedTowardsSecondNode(bool orientedTowardsSecondNode) {
        Source::orientedTowardsSecondNode = orientedTowardsSecondNode;
    }

    bool isIdeal() const {
        return fabs(internalResistance) < EPSILON;
    }

    void setIdeal() {
        Source::internalResistance = 0;
    }

    void setValue(double value) {
        Source::value = value;
    }

    double getValue() const {
        return value;
    }
};

class VoltageSource: public Source {
public:
    VoltageSource(const string &name, double value, bool orientedTowardsSecondNode, double internalResistance = 0) :
    Source(name, value, orientedTowardsSecondNode) {
        this->setInternalResistance(internalResistance);
    }
};

class CurrentSource: public Source {
public:
    CurrentSource(const string &name, double value, bool orientedTowardsSecondNode, double internalResistance = HUGE_VAL) :
    Source(name, value, orientedTowardsSecondNode) {
        this->setInternalResistance(internalResistance);
    }
};

class Resistor: public Component {
    double value;
public:
    Resistor(const string &name, double value) : Component(name) {};

    void setValue(double value) {
        Resistor::value = value;
    }

    double getValue() const {
        return value;
    }
};

class Instrument: public Component {
    bool orientedTowardsSecondNode;
    bool ideal;
    double displayedValue;
public:
    Instrument(const string &name, double displayedValue, double internalResistance, bool orientedTowardsSecondNode, bool ideal)
            : Component(name),
              orientedTowardsSecondNode(orientedTowardsSecondNode), ideal(ideal), displayedValue(displayedValue) {};

    bool isOrientedTowardsSecondNode() const {
        return orientedTowardsSecondNode;
    }

    void setOrientedTowardsSecondNode(bool orientedTowardsSecondNode) {
        Instrument::orientedTowardsSecondNode = orientedTowardsSecondNode;
    }

    bool isIdeal() const {
        return ideal;
    }

    void setIdeal(bool ideal) {
        Instrument::ideal = ideal;
    }

    void setDisplayedValue(double displayedValue) {
        Instrument::displayedValue = displayedValue;
    }

    double getDisplayedValue() const {
        return displayedValue;
    }
};

class Voltmeter : public Instrument {
    double internalResistance;

public:
    Voltmeter(const string &name, double displayedValue, double internalResistance, bool orientedTowardsSecondNode,
              bool ideal, double internalResistance1) : Instrument(name, displayedValue, internalResistance,
                                                                   orientedTowardsSecondNode, ideal),
                                                        internalResistance(internalResistance1) {}

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        Voltmeter::internalResistance = internalResistance;
    }
};

class Ampermeter : public Instrument {
    double internalResistance;
public:
    Ampermeter(const string &name, double displayedValue, double internalResistance, bool orientedTowardsSecondNode,
               bool ideal, double internalResistance1) : Instrument(name, displayedValue, internalResistance,
                                                                    orientedTowardsSecondNode, ideal),
                                                         internalResistance(internalResistance1) {}

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        Ampermeter::internalResistance = internalResistance;
    }
};

class Wattmeter : public Instrument {
    double internalResistanceVolt;
    double internalResistanceAmper;
public:
    Wattmeter(const string &name, double displayedValue, double internalResistance, bool orientedTowardsSecondNode,
              bool ideal, double internalResistanceVolt, double internalResistanceAmper) : Instrument(name,
                                                                                                      displayedValue,
                                                                                                      internalResistance,
                                                                                                      orientedTowardsSecondNode,
                                                                                                      ideal),
                                                                                           internalResistanceVolt(
                                                                                                   internalResistanceVolt),
                                                                                           internalResistanceAmper(
                                                                                                   internalResistanceAmper) {}

    double getInternalResistanceVolt() const {
        return internalResistanceVolt;
    }

    void setInternalResistanceVolt(double internalResistanceVolt) {
        Wattmeter::internalResistanceVolt = internalResistanceVolt;
    }

    double getInternalResistanceAmper() const {
        return internalResistanceAmper;
    }

    void setInternalResistanceAmper(double internalResistanceAmper) {
        Wattmeter::internalResistanceAmper = internalResistanceAmper;
    }
};

class Node {
    double voltage;

public:
    double getVoltage() const {
        return voltage;
    }

    void setVoltage(double voltage) {
        Node::voltage = voltage;
    }
};

class Ground : public Node {
public:
    Ground() {
        this->setVoltage(0);
    }
};

class Branch {
    double current;
    pair<Node, Node> nodes;
    vector<Component> components;

public:
    Branch(const pair<Node, Node> &nodes, const vector<Component> &components) : nodes(nodes), components(components) {}

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        Branch::current = current;
    }

    const pair<Node, Node> &getNodes() const {
        return nodes;
    }

    void setNodes(const pair<Node, Node> &nodes) {
        Branch::nodes = nodes;
    }

    const vector<Component> &getComponents() const {
        return components;
    }

    void setComponents(const vector<Component> &components) {
        Branch::components = components;
    }
};

int main() {

}