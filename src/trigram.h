#ifndef TGRAM_H
#define TGRAM_H

#include <limits>
#include <QHash>

struct tgram {
    char gram[3];
    tgram(char const & c0, char const & c1, char const & c2){
        gram[0] = c0;
        gram[1] = c1;
        gram[2] = c2;
    }
};

inline uint qHash(const tgram &t) {
    return (uint) t.gram[0] * 7 + (uint)t.gram[0] * 49; + (uint) t.gram[1] * 343;

}

inline bool operator==(const tgram& lhs, const tgram& rhs) {
    return (lhs.gram[0] == rhs.gram[0] && lhs.gram[1] == rhs.gram[1] && lhs.gram[2] == rhs.gram[2]);
}

inline bool operator<(const tgram& lhs, const tgram& rhs) {
    return (qHash(lhs) < qHash(rhs));
}

#endif // TGRAM_H
