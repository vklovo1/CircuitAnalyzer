#include <iostream>
#include <cmath>
#include <memory>
#include <utility>
#include <forward_list>


using std::string;
using std::pair;
using std::forward_list;

const double EPSILON = 10e-7;

//Source is referred to by its ID. No two sources should have the same ID.
//naturalOrientation means that the + contact is to the right or up, depending on the circuit.
//in a Branch, naturalOrientation = true means the Source is oriented towards the second Node in the Branch

class VoltageSource {
    int id;
    double voltage;
    double internalResistance;
    bool naturalOrientation;
public:
    explicit VoltageSource(int id, double voltage = 10, double internalResistance = 0, bool naturalOrientation = true) {
        setId(id);
        setVoltage(voltage);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    };

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0) throw std::domain_error("IDs start at 0!");
        VoltageSource::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    void setVoltage(double voltage) {
        if(voltage < 0) {
            voltage *= -1;
            toggleOrientation();
        }
        this->voltage = voltage;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0)
            throw std::domain_error("Resistance can't be negative!");
        VoltageSource::internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        this->naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        this->naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = 0;
    }

    //operators

    friend bool operator == (const VoltageSource &e1, const VoltageSource &e2) {
        return e1.id == e2.id;
    }

    friend bool operator != (const VoltageSource &e1, const VoltageSource &e2) {
        return !(e1 == e2);
    }

    friend bool operator < (const VoltageSource &e1, const VoltageSource &e2) {
        return e1.id < e2.id;
    }

    friend bool operator <= (const VoltageSource &e1, const VoltageSource &e2) {
        return e1 < e2 || e1 == e2;
    }

    VoltageSource &operator +=(const VoltageSource &e) { //sources in series
        if(this->naturalOrientation == e.naturalOrientation)
            this->voltage += e.voltage;
        else {
            if(this->voltage > e.voltage)
                this->voltage = this->voltage - e.voltage;
            else {
                this->toggleOrientation();
                this->voltage = this->voltage - e.voltage;
            }
        }
        this->internalResistance += e.internalResistance;

        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const VoltageSource &e) {
        if(e.isNaturalOrientation())
            os << "E" << e.getId() << " (" << e.getVoltage() << "V)+";
        else
            os << "E" << e.getId() << " +(" << e.getVoltage() << "V)";

        return os;
    }
};

//internal resistance of an ideal CurrentSource is infinite... since we don't want to play with huge values, we'll use -1

class CurrentSource {
    int id;
    double current;
    double internalResistance;
    bool naturalOrientation;
public:

    CurrentSource(int id, double current = 1, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setCurrent(current);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::domain_error("IDs start at 0!");
        CurrentSource::id = id;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        if(current < 0) {
            current *= -1;
            this->naturalOrientation = !this->naturalOrientation;
        }
        this->current = current;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        this->internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        CurrentSource::naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1 + internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const CurrentSource &i1, const CurrentSource &i2) {
        return i1.id == i2.id;
    }

    friend bool operator != (const CurrentSource &i1, const CurrentSource &i2) {
        return !(i1 == i2);
    }

    friend bool operator < (const CurrentSource &i1, const CurrentSource &i2) {
        return i1.id < i2.id;
    }

    friend bool operator <= (const CurrentSource &i1, const CurrentSource &i2) {
        return i1 < i2 || i1 == i2;
    }

    friend std::ostream& operator<<(std::ostream& os, const CurrentSource &i)
    {
        if(i.isNaturalOrientation())
            os << "I" << i.getId() << " (" << i.getCurrent() << "A->)";
        else
            os << "I" << i.getId() << " (<-" << i.getCurrent() << "A)";

        return os;
    }
};

//Resistor has an id of -1 by default... this indicates a parasite resistance from sources or instruments
class Resistor {
    int id;
    double resistance;
public:
    Resistor(double resistance = 100, int id = -1){
        setId(id);
        setResistance(resistance);
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0 && id != -1)
            throw std::domain_error("IDs start at 0!");
        Resistor::id = id;
    }

    double getResistance() const {
        return resistance;
    }

    void setResistance(double resistance) {
        if(resistance < 0) throw std::domain_error("Resistance can't be negative!");
        this->resistance = resistance;
    }

    //operators

    friend bool operator == (const Resistor &r1, const Resistor &r2) {
        return r1.id == r2.id;
    }

    friend bool operator != (const Resistor &r1, const Resistor &r2) {
        return !(r1 == r2);
    }

    friend bool operator < (const Resistor &r1, const Resistor &r2) {
        return r1.id < r2.id;
    }

    friend bool operator <= (const Resistor &r1, const Resistor &r2) {
        return r1 < r2 || r1 == r2;
    }

    Resistor &operator +=(const Resistor &r) { //resistors in series
        this->resistance += r.resistance;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Resistor &r)
    {
        os << "R" << r.getId() << " -[" << r.getResistance() << "Ohm]-";
        return os;
    }
};

//ideal Voltmeter has an infinite resistance, we used -1 instead

class Voltmeter {
    int id;
    double voltage;
    double internalResistance;
    bool naturalOrientation;
public:
    explicit Voltmeter(int id, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    };

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0) throw std::domain_error("IDs start at 0!");
        Voltmeter::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    void setVoltage(double voltage) {
        this->voltage = voltage;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        Voltmeter::internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        this->naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        this->naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1+internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const Voltmeter &e1, const Voltmeter &e2) {
        return e1.id == e2.id;
    }

    friend bool operator != (const Voltmeter &e1, const Voltmeter &e2) {
        return !(e1 == e2);
    }

    friend bool operator < (const Voltmeter &e1, const Voltmeter &e2) {
        return e1.id < e2.id;
    }

    friend bool operator <= (const Voltmeter &e1, const Voltmeter &e2) {
        return e1 < e2 || e1 == e2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Voltmeter &v)
    {
        if(v.isNaturalOrientation())
            os << "V" << v.getId() << " ([" << v.getVoltage() << "V])+";
        else
            os << "V" << v.getId() << " +([" << v.getVoltage() << "V])";

        return os;
    }
};

class Ampermeter {
    int id;
    double current;
    double internalResistance;
    bool naturalOrientation;
public:
    Ampermeter(int id, double internalResistance = -1, bool naturalOrientation = true) {
        setId(id);
        setInternalResistance(internalResistance);
        this->naturalOrientation = naturalOrientation;
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::domain_error("IDs start at 0!");
        Ampermeter::id = id;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        this->current = current;
    }

    double getInternalResistance() const {
        return internalResistance;
    }

    void setInternalResistance(double internalResistance) {
        if(internalResistance < 0 && fabs(1 + internalResistance) > EPSILON)
            throw std::domain_error("Resistance can't be negative!");
        this->internalResistance = internalResistance;
    }

    bool isNaturalOrientation() const {
        return naturalOrientation;
    }

    void setNaturalOrientation(bool naturalOrientation) {
        Ampermeter::naturalOrientation = naturalOrientation;
    }

    void toggleOrientation() {
        naturalOrientation = !naturalOrientation;
    }

    bool isIdeal() {
        return fabs(1 + internalResistance) < EPSILON;
    }

    void setIdeal() {
        internalResistance = -1;
    }

    //operators

    friend bool operator == (const Ampermeter &a1, const Ampermeter &a2) {
        return a1.id == a2.id;
    }

    friend bool operator != (const Ampermeter &a1, const Ampermeter &a2) {
        return !(a1 == a2);
    }

    friend bool operator < (const Ampermeter &a1, const Ampermeter &a2) {
        return a1.id < a2.id;
    }

    friend bool operator <= (const Ampermeter &a1, const Ampermeter &a2) {
        return a1 < a2 || a1 == a2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ampermeter &a)
    {
        if(a.isNaturalOrientation())
            os << "A" << a.getId() << " ([" << a.getCurrent() << "A]->)";
        else
            os << "I" << a.getId() << " (<-[" << a.getCurrent() << "A])";

        return os;
    }
};

class Wattmeter {
    Voltmeter voltmeter;
    Ampermeter ampermeter;
    //to be implemented
};

class Node {
    int id;
    double voltage;

public:
    Node() {}

    Node(int id) {
        setId(id);
    }

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::range_error("IDs start at 0!");
        Node::id = id;
    }

    double getVoltage() const {
        return voltage;
    }

    virtual void setVoltage(double voltage) {
        Node::voltage = voltage;
    }

    //operators

    friend bool operator == (const Node &n1, const Node &n2) {
        return n1.id == n2.id;
    }

    friend bool operator != (const Node &n1, const Node &n2) {
        return !(n1 == n2);
    }

    friend bool operator < (const Node &n1, const Node &n2) {
        return n1.id < n2.id;
    }

    friend bool operator <= (const Node &n1, const Node &n2) {
        return n1 < n2 || n1 == n2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Node &n)
    {
        os << "N" << n.getId() << " " << n.getVoltage() << "V";
        return os;
    }
};

class Ground : public Node {
public:
    Ground() {
        Node::setVoltage(0);
    }

    void setVoltage(double voltage) = 0;
};

class Branch {
    int id;
    pair<Node, Node> nodes;
    forward_list<Resistor> resistors;
    forward_list<VoltageSource> voltageSources;
    forward_list<CurrentSource> currentSources;
    double current = 0;
public:

    Branch(int id, const Node &n1, const Node &n2, const forward_list<Resistor> &resistors, const forward_list <VoltageSource> &voltageSources,
           const forward_list <CurrentSource> &currentSources) {
        setId(id);
        setNodes(n1, n2);
        this->resistors = resistors;
        this->voltageSources = voltageSources;
        this->currentSources = currentSources;
    }

    Branch(int id, const Node &n1, const Node &n2) {
        setId(id);
        setNodes(n1, n2);
        resistors = forward_list<Resistor>();
        voltageSources = forward_list<VoltageSource>();
        currentSources = forward_list<CurrentSource>();
    }

    Branch() {}

    //getters and setters

    int getId() const {
        return id;
    }

    void setId(int id) {
        if(id < 0)
            throw std::range_error("IDs start at 0!");
        Branch::id = id;
    }

    double getCurrent() const {
        return current;
    }

    void setCurrent(double current) {
        Branch::current = current;
    }

    void setNodes(const Node &n1, const Node &n2) {
        this->nodes = pair<Node, Node>(n1, n2);
    }

    const Node &getFirstNode() const {
        return nodes.first;
    };

    const Node &getSecondNode() const {
        return nodes.second;
    }

    void setFirstNode(const Node &node) {
        nodes.first = node;
    }

    void setSecondNode(const Node &node) {
        nodes.second = node;
    }

    const forward_list<Resistor> &getResistors() const {
        return resistors;
    }

    forward_list<Resistor> &getResistors() {
        return resistors;
    }

    void setResistors(const forward_list<Resistor> &resistors) {
        Branch::resistors = resistors;
    }

    const forward_list<VoltageSource> &getVoltageSources() const {
        return voltageSources;
    }

    forward_list<VoltageSource> &getVoltageSources() {
        return voltageSources;
    }

    void setVoltageSources(const forward_list<VoltageSource> &voltageSources) {
        Branch::voltageSources = voltageSources;
    }

    const forward_list<CurrentSource> &getCurrentSources() const {
        return currentSources;
    }

    forward_list<CurrentSource> &getCurrentSources() {
        return currentSources;
    }

    void setCurrentSources(const forward_list<CurrentSource> &currentSources) {
        Branch::currentSources = currentSources;
    }

    //utility

    bool hasResistors() {
        return !resistors.empty();
    }

    bool hasVoltageSources() {
        return !voltageSources.empty();
    }

    bool hasCurrentSources() {
        return !currentSources.empty();
    }

    bool isEmpty() {
        return resistors.empty() && voltageSources.empty() && currentSources.empty();
    }

    bool isLoop() {
        return nodes.first == nodes.second;
    }

    bool isEmptyLoop() {
        return isLoop() && isEmpty();
    }

    //operators

    friend bool operator == (const Branch &b1, const Branch &b2) {
        return b1.getId() == b2.getId();
    }

    friend bool operator != (const Branch &b1, const Branch &b2) {
        return !(b1 == b2);
    }

    friend bool operator < (const Branch &b1, const Branch &b2) {
        return b1.getId() < b2.getId();
    }

    friend bool operator <= (const Branch &b1, const Branch &b2) {
        return b1 < b2 || b1 == b2;
    }

    Branch &operator +=(Branch b) { //branches in series
        this->resistors.splice_after(resistors.end(), b.getResistors());
        this->voltageSources.splice_after(voltageSources.end(), b.getVoltageSources());
        this->currentSources.splice_after(currentSources.end(), b.getCurrentSources());
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Branch &b)
    {
        os <<b.getFirstNode() << "-> B" << b.getId() << " " << b.getCurrent() << "A ->" <<b.getSecondNode();
        return os;
    }
};
//int main() {
//return 0;
//}