#include "algorytm.h"
#include "parametry.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>


Przerwa przerwaZPliku(std::string line) {
      short nrPrzerwy, nrMaszyny, czasTrwania;
      int czasRozpoczecia;
      std::stringstream ss;
      char c;
      ss << line;
      ss >> nrPrzerwy;
      ss >> c;
      ss >> nrMaszyny;
      ss >> c;
      ss >> czasTrwania;
      ss >> c;
      ss >> czasRozpoczecia;
      return Przerwa(nrPrzerwy, czasRozpoczecia, czasTrwania, nrMaszyny);
}

Gen genZPliku(std::string line) {
      short czasOp1, czasOp2, nrMOp1, nrMOp2;
      std::stringstream ss;
      char c;
      ss << line;
      ss >> czasOp1;
      ss >> c;
      ss >> czasOp2;
      ss >> c;
      ss >> nrMOp1;
      ss >> c;
      ss >> nrMOp2;
      return Gen(czasOp1, czasOp2, nrMOp1, nrMOp2);
}

std::vector<Gen> wczytajGenyZPliku(std::string plikInstancji) {
      std::fstream file(plikInstancji.c_str(), std::fstream::in);
      std::vector<Gen> uszeregowanie;
      if(!file) {
            std::cout << "Nie udalo sie utworzyc populacji na podstawie podanej instancji!\n";
            return uszeregowanie;
      }

      std::string line;
      std::getline(file, line);
      std::getline(file, line);
      std::stringstream ss;
      ss << line;
      int liczbaZadan;
      ss >> liczbaZadan;

      for(int i = 0; i < liczbaZadan; i++) {
            std::getline(file, line);
            Gen g = genZPliku(line);
            g.nrZadania = i;
            g.operacje[0].nrZadania = g.operacje[1].nrZadania = i;
            uszeregowanie.push_back(g);
      }

      file.close();
      return uszeregowanie;
}

std::vector<Przerwa> ustalPrzerwy(std::string plikInstancji) {
      std::cout << "Ustalam przerwy konserwujace z pliku instancji\n";
      std::vector<Przerwa> przerwy;

      std::fstream file(plikInstancji.c_str(), std::fstream::in);
      if(!file) {
            std::cout << "Nie udalo sie wczytac przerw z pliku lub zadna przerwa konserwujaca nie istnieje!\n";
            return przerwy;
      }

      std::string line;
      std::getline(file, line);
      std::getline(file, line);
      std::stringstream ss;
      ss << line;
      int liczbaZadan;
      ss >> liczbaZadan;
      for(int i = 0; i < liczbaZadan; i++)
            std::getline(file, line);
      while(file.good()) {
            std::getline(file, line);
            przerwy.push_back(przerwaZPliku(line));
      }

      przerwy.erase(przerwy.begin() + przerwy.size() - 1);

      file.close();
      return przerwy;
}

bool kolidujeZPrzerwa(Operacja operacja, Przerwa przerwa) {
      if(operacja.czasRozpoczecia + operacja.czasOperacji < przerwa.czasRozpoczecia || operacja.czasRozpoczecia > przerwa.czasRozpoczecia + przerwa.czasTrwania)
            return false;
      return true;
}

void zapiszDoPliku(std::string wynikInstancji, std::vector<Osobnik> & populacja, int czasPoczatkowy) {
      std::string nrInstancji = wynikInstancji.substr(12);
      for(int i = 0; i < nrInstancji.length(); i++) {
            if(nrInstancji[i] < '0' || nrInstancji[i] > '9') {
                  nrInstancji.insert(i, "");
            }
      }

      std::fstream file(wynikInstancji.c_str(), std::fstream::out | std::fstream::trunc);
      file << "*** INSTANCJA NR " + nrInstancji + " ***\n";


      int min = 200000;
      int index = -1;
      for(int i = 0; i < populacja.size(); i++) {
            if(populacja[i].czasUszeregowania < min) {
                  index = i;
                  min = populacja[i].czasUszeregowania;
            }
      }

      file << intToString(czasPoczatkowy) + " ; " + intToString(min) + "\n";

// maszyna1
      file << "M1: " ;
      Maszyna m = populacja[index].maszyna1;
      for(int i = 0; i < m.operacje.size(); i++) {
            file << "o" + intToString(m.operacje[i].nrOperacji) + "_z" + intToString(m.operacje[i].nrZadania) + ", " +
            intToString(m.operacje[i].czasRozpoczecia) + ", " + intToString(m.operacje[i].czasOperacji) + "; ";
      }
      file << "\n";

      int iloscPrzerwM1 = m.przerwy.size();
      int czasPrzerwM1 = 0;

      for(int i = 0; i < m.przerwy.size(); i++) {
            file << "maint_" + intToString(i+1) + "_M1 " + intToString(m.przerwy[i].czasRozpoczecia) + ", " +
            intToString(m.przerwy[i].czasTrwania) + "; ";
            czasPrzerwM1 += m.przerwy[i].czasTrwania;
      }
      file << "\n";

      int nrPrzerwy = 0;
      int nrIdle1 = 0;
      int czasIdle1 = 0;
      int ostatniCzas = 0;
      for(int i = 0; i < m.operacje.size(); i++) {
            while(m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas < m.operacje[i].czasOperacji) {
                  file << "idle_" + intToString(nrIdle1) + ", " + intToString(ostatniCzas)
                  + ", " + intToString(m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas) + "; ";
                  czasIdle1 += m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas;
                  ostatniCzas = m.przerwy[nrPrzerwy].czasRozpoczecia + m.przerwy[nrPrzerwy].czasTrwania;
                  nrIdle1++;
                  nrPrzerwy++;
                  if(nrPrzerwy == m.przerwy.size())
                        break;
            }
            if(nrPrzerwy == m.przerwy.size())
                  break;
            ostatniCzas += m.operacje[i].czasOperacji;
      }
      file << "\n";

 // maszyna2
      file << "M2: " ;
      m = populacja[index].maszyna2;
      for(int i = 0; i < m.operacje.size(); i++) {
            file << "o" + intToString(m.operacje[i].nrOperacji) + "_z" + intToString(m.operacje[i].nrZadania) + ", " +
            intToString(m.operacje[i].czasRozpoczecia) + ", " + intToString(m.operacje[i].czasOperacji) + "; ";
      }
      file << "\n";

      int iloscPrzerwM2 = m.przerwy.size();
      int czasPrzerwM2 = 0;

      for(int i = 0; i < m.przerwy.size(); i++) {
            file << "maint_" + intToString(i+1) + "_M2 " + intToString(m.przerwy[i].czasRozpoczecia) + " " +
            intToString(m.przerwy[i].czasTrwania) + " ";
            czasPrzerwM2 += m.przerwy[i].czasTrwania;
      }
      file << "\n";

      nrPrzerwy = 0;
      int nrIdle2 = 0;
      int czasIdle2 = 0;
      ostatniCzas = 0;
      for(int i = 0; i < m.operacje.size(); i++) {
            while(m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas < m.operacje[i].czasOperacji) {
                  file << "idle_" + intToString(nrIdle2) + ", " + intToString(ostatniCzas)
                  + ", " + intToString(m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas) + "; ";
                  czasIdle2 += m.przerwy[nrPrzerwy].czasRozpoczecia - ostatniCzas;
                  ostatniCzas = m.przerwy[nrPrzerwy].czasRozpoczecia + m.przerwy[nrPrzerwy].czasTrwania;
                  nrIdle2++;
                  nrPrzerwy++;
                  if(nrPrzerwy == m.przerwy.size())
                        break;
            }
            if(nrPrzerwy == m.przerwy.size())
                  break;
            ostatniCzas += m.operacje[i].czasOperacji;
      }
      file << "\n";
      file << iloscPrzerwM1 << " ; " << czasPrzerwM1 << "\n";
      file << iloscPrzerwM2 << " ; " << czasPrzerwM2 << "\n";
      file << nrIdle1 << " ; " << czasIdle1 << "\n";
      file << nrIdle2 << " ; " << czasIdle2 << "\n";
}

std::vector<Osobnik> stworzPopulacje(std::vector<Gen> usz, std::vector<Przerwa> & przerwy) {


      std::vector<Gen> uszeregowanie = usz;
      std::vector<Osobnik> populacja;
      // funkcja tworz¹ca osobników

      std::vector<Przerwa> przerwyM1;
      std::vector<Przerwa> przerwyM2;


      for(int i = 0; i < przerwy.size(); i++) {
            if(przerwy[i].nrMaszyny == 1) {przerwyM1.push_back(przerwy[i]);}
            else {przerwyM2.push_back(przerwy[i]);}
      }

      for(int i = 0; i < ROZMIAR_POPULACJI; i++) {
            Osobnik o = Osobnik(uszeregowanie, przerwyM1, przerwyM2);
            std::random_shuffle(uszeregowanie.begin(), uszeregowanie.end());
            populacja.push_back(o);
      }

      for(int i = 0; i < populacja.size(); i++)
            populacja[i].rozlozOperacje();

      return populacja;
}


// funkcja naprawiajaca

void naprawPopulacje(std::vector<Osobnik> & populacja, std::vector<Gen> & uszeregowanie) {
      for(int i = 0; i < populacja.size(); i++) {
            naprawOsobnika(populacja[i], uszeregowanie);
            populacja[i].rozlozOperacjeNaMaszynach();
      }
}

void naprawOsobnika(Osobnik & o, std::vector<Gen> & uszeregowanie) {
      int liczbaOperacjiNaMaszynie = o.maszyna1.operacje.size();
      std::vector<int> indeksy;
      std::vector<int> indeksyBrak;
      int M[liczbaOperacjiNaMaszynie];
      bool nadmiar[liczbaOperacjiNaMaszynie];
      //=================================
      // MASZYNA 1 ======================
      //=================================
      for(int i = 0; i < liczbaOperacjiNaMaszynie; i++)
            nadmiar[i] = false;
      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++)
            M[j] = 0;

      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++) {
            M[o.maszyna1.operacje[j].nrZadania]++;
            if(nadmiar[o.maszyna1.operacje[j].nrZadania] == false)
                  nadmiar[o.maszyna1.operacje[j].nrZadania] = true;
            else
                  indeksy.push_back(j);
      }
      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++) {
            if(M[j] == 0)
                  indeksyBrak.push_back(j);
      }
      for(int j = 0; j < indeksy.size(); j++) {
            if(uszeregowanie[indeksyBrak[j]].operacje[0].nrMaszyny == 1)
                  o.maszyna1.operacje[indeksy[j]] = uszeregowanie[indeksyBrak[j]].operacje[0];
            else
                  o.maszyna1.operacje[indeksy[j]] = uszeregowanie[indeksyBrak[j]].operacje[1];
      }
      //=================================
      // MASZYNA 2 ======================
      //=================================
      indeksy.clear();
      indeksyBrak.clear();

      for(int i = 0; i < liczbaOperacjiNaMaszynie; i++)
            nadmiar[i] = false;
      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++)
            M[j] = 0;

      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++) {
            M[o.maszyna2.operacje[j].nrZadania]++;
            if(nadmiar[o.maszyna2.operacje[j].nrZadania] == false)
                  nadmiar[o.maszyna2.operacje[j].nrZadania] = true;
            else
                  indeksy.push_back(j);
      }
      for(int j = 0; j < liczbaOperacjiNaMaszynie; j++) {
            if(M[j] == 0)
                  indeksyBrak.push_back(j);
      }
      for(int j = 0; j < indeksy.size(); j++) {
            if(uszeregowanie[indeksyBrak[j]].operacje[0].nrMaszyny == 2)
                  o.maszyna2.operacje[indeksy[j]] = uszeregowanie[indeksyBrak[j]].operacje[0];
            else
                  o.maszyna2.operacje[indeksy[j]] = uszeregowanie[indeksyBrak[j]].operacje[1];
      }
}

// krzyzowanie

void krzyzuj(std::vector<Osobnik> & populacja) {
      while(populacja.size() < ROZMIAR_POPULACJI * 2) {
            int idx1 = rand() % populacja.size();
            int idx2 = -1;
            do {
                  idx2 = rand() % populacja.size();
            } while(idx1 == idx2);

            Osobnik o1 = populacja[idx1];
            Osobnik o2 = populacja[idx2];

            Maszyna o1M1, o1M2, o2M1, o2M2;
            o1M1 = o1.maszyna1; o1M2 = o1.maszyna2; o2M1 = o2.maszyna1; o2M2 = o2.maszyna2;
            Maszyna nowa1 = o1M1, nowa2 = o1M2;
            for(int i = 0; i < o1M1.operacje.size(); i++) {
                  if(i < o1M1.operacje.size() / 4 || i > o1M1.operacje.size() * 3 / 4) {
                        nowa1.operacje[i] = o1M1.operacje[i];
                        nowa2.operacje[i] = o2M2.operacje[i];
                  }
                  else {
                        nowa1.operacje[i] = o2M1.operacje[i];
                        nowa2.operacje[i] = o1M2.operacje[i];
                  }
            }

            Osobnik o;
            o.maszyna1 = nowa1;
            o.maszyna2 = nowa2;

            populacja.push_back(o);
      }
}

// selekcja turniejowa

void selekcjaPopulacji(std::vector<Osobnik> & populacja) {

    srand(time(NULL));
    Osobnik tmp1, tmp2;
    int a,b;
    std::vector<Osobnik> nowaPopulacja;
    while(populacja.size() != 0){
        a = rand() % populacja.size();
        b = rand() % populacja.size();

        tmp1 = populacja[a];
        tmp2 = populacja[b];

        populacja.erase(populacja.begin()+a);
        populacja.erase(populacja.begin()+b);

        if(tmp1.czasUszeregowania <= tmp2.czasUszeregowania){
                nowaPopulacja.push_back(tmp1);
        }else{
                nowaPopulacja.push_back(tmp2);
        }
    }

    //przepisanie
     for(int i = 0; i < nowaPopulacja.size(); i++) {
            populacja.push_back(nowaPopulacja.at(i));
      }
}


// mutacja

void mutuj(int mutacja, std::vector<Osobnik> & populacja){
    srand(time(NULL));
    for(int i=0; i<populacja.size(); i++){
        if (mutacja > 1) {
            int  x = rand() % 100;
            if (x < mutacja) {
                int a = rand() % populacja.size();
                int b = rand() % populacja.size();
                int c = rand() % 2;

                zamienGeny(populacja[i], a, b, c);

            }
        }
    }
}

void zamienGeny(struct Osobnik & osobnik, int a, int b, int c){
    Operacja tmp;
    if(c == 0){
          tmp = osobnik.maszyna1.operacje[a];
          osobnik.maszyna1.operacje[a] = osobnik.maszyna1.operacje[b];
          osobnik.maszyna1.operacje[b] = tmp;
    }else if(c == 1){
          tmp = osobnik.maszyna1.operacje[a];
          osobnik.maszyna2.operacje[a] = osobnik.maszyna1.operacje[b];
          osobnik.maszyna2.operacje[b] = tmp;
    }
}
