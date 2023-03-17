/*#include <vector>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include "satBasicDef.h"
using namespace std;



class Problem {

private:

    typedef vector<PL> Clause;
    typedef vector<Clause> MC;

    MC root;

    vector<L>  model;
    vector<L*> stack;

    uint64_t numClauses;
    LID      numVars;

    uint nextIndex;
    uint level;

    inline void propSetTrue(PL* pl) {

        stack.push_back(pl->getRef());

        pl->setTrue();
    }

    static inline bool CisTrue(const Clause& c) {

        return any_of(c.begin(), c.end(), [](const PL& pl) -> bool {
            return pl.isTrue();
        });
    }

    void checkModel() {

        if (all_of(root.begin(), root.end(), CisTrue))
            return;

        exit(-1);
    }

    bool clauseConflict(Clause& c) {

        bool one = false;

        PL* lst = nullptr;

        for (PL& pl: c)
            if (pl.isUndef()) {

                if (lst != nullptr)
                    return false;

                lst = &pl;

            } else if (pl.isTrue())
                return false;

        if (lst == nullptr)
            return true;

        propSetTrue(lst);
        return false;
    }

public:


    void readInput() {

        // Skip comments
        char c;
        cin >> c;
        while (c == 'c') {
            while (c != '\n')
                cin >> c;
            cin >> c;
        }

        // Read "cnf numVars numClauses"
        string aux;
        cin >> aux >> numVars >> numClauses;

        nextIndex = 0;
        level = 0;

        root = MC();
        stack = vector<L*>();
        model = vector<L>();

        stack.resize(numVars);

        for (LID id = 1; id <= numVars; ++id)
            model.emplace_back(id, UNDEF);

        // Read clauses
        for (uint i = 0; i < numClauses; ++i) {

            int lit;

            while (cin >> lit and lit != 0) {

                if (lit > 0)
                    root[i].emplace_back(&model[lit -1], TRUE);
                else
                    root[i].emplace_back(&model[-lit -1], FALSE);
            }
        }
    }

};#include <vector>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include "satBasicDef.h"
using namespace std;



class Problem {

private:

    typedef vector<PL> Clause;
    typedef vector<Clause> MC;

    MC root;

    vector<L>  model;
    vector<L*> stack;

    uint64_t numClauses;
    LID      numVars;

    uint nextIndex;
    uint level;

    inline void propSetTrue(PL* pl) {

        stack.push_back(pl->getRef());

        pl->setTrue();
    }

    static inline bool CisTrue(const Clause& c) {

        return any_of(c.begin(), c.end(), [](const PL& pl) -> bool {
            return pl.isTrue();
        });
    }

    void checkModel() {

        if (all_of(root.begin(), root.end(), CisTrue))
            return;

        exit(-1);
    }

    bool clauseConflict(Clause& c) {

        bool one = false;

        PL* lst = nullptr;

        for (PL& pl: c)
            if (pl.isUndef()) {

                if (lst != nullptr)
                    return false;

                lst = &pl;

            } else if (pl.isTrue())
                return false;

        if (lst == nullptr)
            return true;

        propSetTrue(lst);
        return false;
    }

public:


    void readInput() {

        // Skip comments
        char c;
        cin >> c;
        while (c == 'c') {
            while (c != '\n')
                cin >> c;
            cin >> c;
        }

        // Read "cnf numVars numClauses"
        string aux;
        cin >> aux >> numVars >> numClauses;

        nextIndex = 0;
        level = 0;

        root = MC();
        stack = vector<L*>();
        model = vector<L>();

        stack.resize(numVars);

        for (LID id = 1; id <= numVars; ++id)
            model.emplace_back(id, UNDEF);

        // Read clauses
        for (uint i = 0; i < numClauses; ++i) {

            int lit;

            while (cin >> lit and lit != 0) {

                if (lit > 0)
                    root[i].emplace_back(&model[lit -1], TRUE);
                else
                    root[i].emplace_back(&model[-lit -1], FALSE);
            }
        }
    }

};*/