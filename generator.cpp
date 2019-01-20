#include "generator.h"
#include "parametry.h"

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

void generujInstancje() {
      for(int i = NUMER_INSTANCJI - 1; i < NUMER_INSTANCJI; i++) {
            std::string fileName = "instancja" + intToString(i+1) + ".txt";
            std::fstream f(fileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
            int liczbaZadan = LICZBA_ZADAN;
            std::string instance = "*** INSTANCJA NR " +  intToString(i + 1) + " ***\n";
            instance += intToString(liczbaZadan) + "\n";

            int sumaCzasowOperacji = 0;
            for(int j = 0; j < liczbaZadan; j++) {
                  int t1 = rand() % (MAKSYMALNY_CZAS_OPERACJI - MINIMALNY_CZAS_OPERACJI) + MINIMALNY_CZAS_OPERACJI;
                  int t2 = rand() % (MAKSYMALNY_CZAS_OPERACJI - MINIMALNY_CZAS_OPERACJI) + MINIMALNY_CZAS_OPERACJI;
                  std::vector<int> v; v.push_back(1); v.push_back(2);
                  int r = rand() % v.size();
                  int p1 = v[r]; v.erase(v.begin() + r);
                  int p2 = v[0];
                  std::string tempStr = intToString(t1) + ";" + intToString(t2) + ";" + intToString(p1) + ";" + intToString(p2) + "\n";
                  instance += tempStr;
                  sumaCzasowOperacji += t1 + t2;
            }

            int liczbaPrzerw = LICZBA_PRZERW_KONSERWUJACYCH * liczbaZadan;
            int tMaksymalne = MAKSYMALNY_CZAS_ROZPOCZECIA_PRZERWY * sumaCzasowOperacji * 2; // * 2 poniewaz przerwy rozbija sie na dwie obie maszyny
            int tAktualne[2] = {0, 0};
            int minPrzedzialMiedzyPrzerwami = MAKSYMALNY_CZAS_PRZERWY;
            int maxPrzedzialMiedzyPrzerwami = tMaksymalne / liczbaPrzerw;
            for(int j = 0; j < liczbaPrzerw; j++) {
                  int nrPrzerwy = j+1;
                  int nrMaszyny = rand()%2 + 1;
                  int tPrzerwy = rand() % (MAKSYMALNY_CZAS_PRZERWY - MINIMALNY_CZAS_PRZERWY) + MINIMALNY_CZAS_PRZERWY;
                  int tMiedzyPrzerwami = rand() % (maxPrzedzialMiedzyPrzerwami - minPrzedzialMiedzyPrzerwami) + minPrzedzialMiedzyPrzerwami;
                  tAktualne[nrMaszyny - 1] += tMiedzyPrzerwami;
                  int tRozpoczeciaPrzerwy = tAktualne[nrMaszyny - 1];

                  std::string tempStr = intToString(nrPrzerwy) + ";" + intToString(nrMaszyny) + ";" + intToString(tPrzerwy)
                  + ";" + intToString(tRozpoczeciaPrzerwy) + "\n";

                  instance += tempStr;
            }
            f << instance;
            f.close();
      }
}



