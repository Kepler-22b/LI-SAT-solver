#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <cstdint>
using namespace std;

class Lit;

typedef vector<Lit> Clause;
typedef vector<Clause> PModel;
typedef uint32_t LID;

enum LST : int8_t {
    FALSE = -1,
    UNDEF = 0,
    TRUE = 1
};

char stateToSymbol(LST);

class Lit {

private:

    LID id;
    LST st;

public:

    Lit(LID id, LST st) : id(id), st(st) {}

    [[nodiscard]] LST state() const {

        return st;
    }

    [[nodiscard]] LID getId() const {

        return id;
    }

    Lit reversed() {

        return {id, (LST)(-st)};
    }
};


uint64_t numClauses;
uint64_t numVars;

PModel clauses;
vector<LID> model;
vector<Lit> modelStack;

uint indexOfNextLitToPropagate;
uint decisionLevel;

void setLitTrue(Lit l) {

    modelStack.push_back(l);

    model[l.getId()] = l.state();
}

void setLitTrue(LID id, LST st) {

    modelStack.emplace_back(id, st);

    model[id] = st;
}

LST currentModelValue(Lit l) {

    if (model[l.getId()] == UNDEF)
        return UNDEF;

    if (model[l.getId()] == l.state())
        return TRUE;

    return FALSE;
}

bool someLitTrue(Clause& c) {

    bool (*g)(Lit) = [](Lit l) -> bool {
        return currentModelValue(l) == TRUE;
    };

    return any_of(c.begin(), c.end(), g);
}

void readInput();

void unitClauses();

void checkModel();

bool propagateGivesConflict () {

    while ( indexOfNextLitToPropagate < modelStack.size() ) {

        ++indexOfNextLitToPropagate;

        for (const Clause& c: clauses) {

            int numUndefs = 0;

            Lit lastLitUndef = Lit(0, UNDEF);

            for (Lit l: c)
                switch (currentModelValue(l)) {
                    case FALSE:
                    break;
                    case UNDEF:
                        ++numUndefs;
                        lastLitUndef = l;
                    break;
                    case TRUE:
                        goto cont;
                }

            if (numUndefs == 0)
                return true; // conflict! all lits false
            else if (numUndefs == 1)
                setLitTrue(lastLitUndef);

            cont: continue;
        }
    }

    return false;
}

void backtrack(){

    uint i = modelStack.size() -1;

    Lit l = Lit(0, UNDEF);

    while (modelStack[i].getId() != 0) { // 0 is the DL mark

        l = modelStack[i];
        model[l.getId()] = UNDEF;
        modelStack.pop_back();
        --i;
    }

    // at this point, lit is the last decision
    modelStack.pop_back(); // remove the DL mark
    --decisionLevel;
    indexOfNextLitToPropagate = modelStack.size();
    setLitTrue(l.reversed());  // reverse last decision
}


// Heuristic for finding the next decision literal:
LID getNextDecisionLiteral(){

    for (uint i = 1; i <= numVars; ++i) // stupid heuristic:
        if (model[i] == UNDEF)
            return i;  // returns first UNDEF var, positively

    return 0; // reurns 0 when all literals are defined
}

int main(){

    readInput(); // reads numVars, numClauses and clauses

    model.resize(numVars + 1,UNDEF);
    indexOfNextLitToPropagate = 0;
    decisionLevel = 0;

    unitClauses();

    // DPLL algorithm
    while (true) {

        while ( propagateGivesConflict() ) {

            if ( decisionLevel == 0) {

                cout << "UNSATISFIABLE" << endl;
                exit(10);
            }

            backtrack();
        }

        uint decisionLit = getNextDecisionLiteral();

        if (decisionLit == 0) {

            checkModel();
            cout << "SATISFIABLE" << endl;
            exit(20);
        }

        // start new decision level:
        modelStack.emplace_back(0, UNDEF);  // push mark indicating new DL
        ++indexOfNextLitToPropagate;
        ++decisionLevel;
        setLitTrue(decisionLit, TRUE);    // now push decisionLit on top of the mark
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
                cout << "UNSATISFIABLE" << endl;
                exit(10);
            case UNDEF:
                setLitTrue(c[0]);
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

void checkModel() {

    for (Clause& c: clauses)
        if (not someLitTrue(c))
            printErrorTerm(c);
}