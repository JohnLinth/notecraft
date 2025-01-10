#ifndef KURS_H
#define KURS_H

#include <string>
using std::string;

class Kurs
{
public:
    Kurs();
    Kurs(string bezeichnung);
    Kurs(string bezeichnung, double bewertung, double etcs);

    string getBezeichnung() const;

    double getBewertung() const;

    double getEcts() const;

private:
    string bezeichnung;
    double bewertung;
    double ects;
};

#endif // KURS_H
