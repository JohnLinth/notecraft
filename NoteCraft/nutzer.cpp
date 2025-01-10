#include "nutzer.h"

Nutzer::Nutzer()
{
    kurse.push_back(Kurs("Betriebssysteme", 2.7, 5.0));
    kurse.push_back(Kurs("Programmieren 2", 2.3, 7.5));
    kurse.push_back(Kurs("Software Engineering", 1.3, 5.0));
    kurse.push_back(Kurs("Theoretische Informatik", 2.3, 5.0));

}

vector<Kurs> Nutzer::getKurse() const
{
    return kurse;
}

void Nutzer::kursHinzufuegen(Kurs kurs)
{
    kurse.push_back(kurs);
}

