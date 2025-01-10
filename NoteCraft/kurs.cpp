#include "kurs.h"

Kurs::Kurs()
    : bezeichnung("Default-Kurs"), bewertung(0.0), ects(0.0)
{

}

Kurs::Kurs(string bezeichnung)
    : bezeichnung(bezeichnung), bewertung(0.0), ects(0.0)
{

}

Kurs::Kurs(string bezeichnung, double bewertung, double etcs)
    : bezeichnung(bezeichnung),bewertung(bewertung), ects(etcs)
{

}

string Kurs::getBezeichnung() const
{
    return bezeichnung;
}

double Kurs::getBewertung() const
{
    return bewertung;
}

double Kurs::getEcts() const
{
    return ects;
}


