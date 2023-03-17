/*

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <cmath>

using namespace std;

enum LST : int8_t;
typedef uint16_t LID;

class Lit {

private:

    LID id;
    LST st;

public:

    inline Lit(LID id, LST st) : id(id), st(st) {}

    [[nodiscard]] inline LST state() const {
        return st;
    }

    [[nodiscard]] inline LID getId() const {
        return id;
    }

    inline void reverse() {
        st = (LST)(-st);
    }
};

typedef vector<Lit> Clause;
typedef vector<Clause> Problem;

enum LST : int8_t {
    FALSE = -1,
    UNDEF = 0,
    TRUE = 1
};

uint64_t numClauses;
LID numVars;

Problem clauses;
vector<LST> model;
vector<Lit> modelStack;

vector<vector<uint64_t>> cLitTrue;
vector<vector<uint64_t>> cLitFalse;

vector<double> value;

uint nextIndex;
uint level;

uint64_t back = 0;

void setLit(Lit);

void setLit(LID, LST);

LST currentModelValue(Lit);

char stateToSymbol(LST);

int printSat();

int printNotSat();

void readInput();

void unitClauses();

void checkModel();

void backtrack();

bool clauseConflict(const Clause&);

bool propagateGivesConflict () {

    for (; nextIndex < modelStack.size(); ++nextIndex) {

        Lit &l = modelStack[nextIndex];

        switch (l.state()) {
            case FALSE:
                for (uint64_t cid: cLitTrue[l.getId()])
                    if (clauseConflict(clauses[cid]))
                        return true;
                break;
            case UNDEF:
                break;
            case TRUE:
                for (uint64_t cid: cLitFalse[l.getId()])
                    if (clauseConflict(clauses[cid]))
                        return true;
                break;
        }
    }

    return false;
}

LID nextDecision() {

    uint8_t count = 0;

    LID ret    = 0;
    double val = 0;

    for (const Clause &c: clauses) {

        if (any_of(c.begin(), c.end(), [] (const Lit& l) -> bool {
            return model[l.getId()] == l.state();
        })) continue;

        for (const Lit &l: c)
            if (model[l.getId()] == UNDEF && value[l.getId()] > val) {

                ret = l.getId();
                val = value[l.getId()];

                ++count;

                if (count > 5)
                    return ret;
            }

        if (count != 0)
            continue;

        cout << "Unexpected error" << endl;
        exit(-3);
    }

    if (count != 0)
        return ret;

    //no UNDEF lit found: terminate program

    checkModel();
    exit(printSat());
}

void makeDecision() {

    LID id = nextDecision();

    modelStack.emplace_back(0, UNDEF);
    ++nextIndex;
    ++level;

    setLit(id, FALSE);
}

void initClauseIndex() {

    cLitTrue.resize(numVars + 1, vector<uint64_t>());
    cLitFalse.resize(numVars + 1, vector<uint64_t>());

    for (uint64_t i = 0; i < clauses.size(); ++i)
        for (Lit l: clauses[i])
            switch (l.state()) {

                case FALSE:
                    cLitFalse[l.getId()].push_back(i);
                    break;
                case UNDEF:
                    break;
                case TRUE:
                    cLitTrue[l.getId()].push_back(i);
                    break;
            }
}

void compPriority() {

    fill(value.begin(), value.end(), 1);

    auto i = (double)numClauses;

    for (const Clause& c: clauses) {

        for (const Lit &l: c)
            value[l.getId()] *= log2(i);

        for (const Lit& l: c)
            value[l.getId()] *= 10;

        --i;
    }

}

int main(){

    readInput();

    model.resize(numVars + 1,UNDEF);

    nextIndex = 0;
    level = 0;

    value.resize(numVars + 1);

    initClauseIndex();

    unitClauses();

    compPriority();

    while (true) {

        while (propagateGivesConflict()) {

            if (level == 0)
                exit(printNotSat());

            backtrack();
        }

        makeDecision();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

void unitClauses() {

    // Take care of initial unit clauses, if any
    for (Clause& c: clauses) {

        if (c.size() != 1)
            continue;

        switch (currentModelValue(c[0])) {

            case FALSE:
                exit(printNotSat());
            case UNDEF:
                setLit(c[0]);
                break;
            case TRUE:
                break;
        }
    }
}

void readInput() {

    // Skip comments
    char c = cin.get();

    while (c == 'c') {

        while (c != '\n')
            c = cin.get();

        c = cin.get();
    }

    // Read "cnf numVars numClauses"
    string aux;
    cin >> aux >> numVars >> numClauses;

    clauses.resize(numClauses);

    // Read clauses
    for (uint i = 0; i < numClauses; ++i) {

        int lit;

        while (cin >> lit and lit != 0) {

            if (lit > 0)
                clauses[i].emplace_back(lit, TRUE);
            else
                clauses[i].emplace_back(-lit, FALSE);
        }
    }
}

char stateToSymbol(LST st) {

    switch (st) {
        case FALSE:
            return '-';
        case TRUE:
            return '+';
        case UNDEF:
            return ' ';
        default:
            return '?';
    }
}

void printErrorTerm(const Clause& c) {

    cout << "Error in model, clause is not satisfied:";

    for (Lit l: c)
        cout << "[" << stateToSymbol(l.state()) << "]" << l.getId() << " ";

    cout << endl;
    exit(1);
}

bool someLitTrue(const Clause& c) {

    return any_of(c.begin(), c.end(), [](Lit l) -> bool {
        return currentModelValue(l) == TRUE;
    });
}

void checkModel() {

    for (Clause& c: clauses)
        if (not someLitTrue(c))
            printErrorTerm(c);
}

int printSat() {

    cout << "SATISFIABLE" << ' ' << back << endl;
    return 20;
}

int printNotSat() {

    cout << "UNSATISFIABLE" << ' ' << back << endl;
    return 10;
}

LST currentModelValue(Lit l) {

    if (model[l.getId()] == UNDEF)
        return UNDEF;

    if (model[l.getId()] == l.state())
        return TRUE;

    return FALSE;
}

void backtrack() {

    Lit l = Lit(0, UNDEF);

    for (auto it = modelStack.rbegin();
         it != modelStack.rend();
         ++it, modelStack.pop_back()
            ) {

        if (it->getId() == 0)
            break;

        l = *it;
        model[l.getId()] = UNDEF;
    }

    modelStack.pop_back();
    --level;
    nextIndex = modelStack.size();

    l.reverse();
    setLit(l);

    ++back;
}

void setLit(Lit l) {

    modelStack.push_back(l);

    model[l.getId()] = l.state();
}

void setLit(LID id, LST st) {

    modelStack.emplace_back(id, st);

    model[id] = st;
}

bool clauseConflict(const Clause& c) {

    bool one = false;

    Lit lastLitUndef = Lit(0, UNDEF);

    for (Lit l: c)
        switch (currentModelValue(l)) {
            case FALSE:
                break;
            case UNDEF:

                if (one)
                    return false;
                one = true;

                lastLitUndef = l;
                break;
            case TRUE:
                return false;
        }

    if (not one)
        return true;

    setLit(lastLitUndef);
    return false;
}*/