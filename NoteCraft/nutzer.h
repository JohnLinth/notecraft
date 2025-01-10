#ifndef NUTZER_H
#define NUTZER_H
#include "kurs.h"
#include <vector>
using std::vector;

class Nutzer
{
public:
    Nutzer();

    vector<Kurs> getKurse() const;

    void kursHinzufuegen(Kurs kurs);
private:
    vector<Kurs> kurse;
};

#endif // NUTZER_H
