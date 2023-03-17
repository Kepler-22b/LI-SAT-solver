//
// Created by kepler-22b on 10/03/23.
//

#ifndef LI_SAT_SOLVER_VDEF_H

#include <iostream>
#include <cstdint>

enum LST : int8_t {
    FALSE = -1,
    UNDEF = 0,
    TRUE = 1
};

typedef uint16_t LID;

static std::string stateToSymbol(LST st) {

    switch (st) {
        case FALSE:
            return "[-]";
        case TRUE:
            return "[+]";
        case UNDEF:
            return "[ ]";
        default:
            return "[?]";
    }
}

class L {

private:

    LID id;
    LST st;

public:

    inline L(LID id, LST st) : id(id), st(st) {}

    [[nodiscard]] inline LID getId() const {

        return id;
    }

    [[nodiscard]] inline LST getSt() const {

        return st;
    }

    inline void reverse() {

        st = (LST)(-st);
    }

    inline void set(LST s) {

        st = s;
    }

    inline bool operator == (const L& l) const {

        return id == l.id;
    }

    inline bool operator < (const L& l) const {

        return id < l.id;
    }

    friend  std::ostream& operator << (std::ostream& os, const L& lit);
};

std::ostream& operator << (std::ostream& os, const L& lit) {

    return os << stateToSymbol(lit.st) << lit.id;
}

std::ostream& operator << (std::ostream& os, const LST& st) {

    return os << stateToSymbol(st);
}

class PL {

private:

    float val;
    LID   id;
    LST*  ref;
    LST   st;

public:

    inline PL() : id(), ref(&st), st(), val(0) {}

    inline PL(LID id, LST *ref, LST st) : id(id), ref(ref), val(0), st(st) {}

    [[nodiscard]] inline bool isTrue() const {

        return *ref == st;
    }

    [[nodiscard]] inline static bool isTrueS(const PL& pl) {

        return *pl.ref == pl.st;
    }

    [[nodiscard]] inline static bool isFalseS(const PL& pl) {

        if (*pl.ref == UNDEF)
            return false;

        return *pl.ref != pl.st;
    }

    [[nodiscard]] inline bool isUndef() const {

        return *ref == UNDEF;
    }

    [[nodiscard]] inline LST getSt() const {

        return st;
    }

    [[nodiscard]] inline LID getId() const {

        return id;
    }

    [[nodiscard]] inline float getValue() const {

        return val;
    }

    inline void setValue(float v) {

        val = v;
    }
};

#define LI_SAT_SOLVER_VDEF_H

#endif //LI_SAT_SOLVER_VDEF_H
