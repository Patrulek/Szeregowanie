#ifndef ALGORYTM_H_INCLUDED
#define ALGORYTM_H_INCLUDED

#include <vector>
#include <string>

struct Przerwa {
      short nrPrzerwy;
      int czasRozpoczecia;
      short czasTrwania;
      short nrMaszyny;

      Przerwa(short nrP, int roz, short trw, short nrM) : nrPrzerwy(nrP), czasRozpoczecia(roz), czasTrwania(trw), nrMaszyny(nrM) {};
};

struct Operacja {
      int czasRozpoczecia;
      short czasOperacji;
      short nrMaszyny;
      short nrOperacji;
      short nrZadania;

      Operacja(short nrZadania, short nrOperacji, int czasRozpoczecia, short czasOperacji, short nrMaszyny) : nrZadania(nrZadania),
      nrOperacji(nrOperacji), czasRozpoczecia(czasRozpoczecia), czasOperacji(czasOperacji), nrMaszyny(nrMaszyny) {};
      Operacja() {};
};

struct Gen {
      short nrZadania;
      Operacja operacje[2];
      int czasZakonczeniaOperacjiJeden;

      Gen(short nrZ, short czasOp1, short czasOp2, short nrMOp1, short nrMOp2) {
            nrZadania = nrZ;
            operacje[0] = Operacja(nrZadania, 1, 0, czasOp1, nrMOp1);
            operacje[1] = Operacja(nrZadania, 2, 0, czasOp2, nrMOp2);
            czasZakonczeniaOperacjiJeden = czasOp1;
      }

      Gen(short czasOp1, short czasOp2, short nrMOp1, short nrMOp2) {
            operacje[0] = Operacja(nrZadania, 1, 0, czasOp1, nrMOp1);
            operacje[1] = Operacja(nrZadania, 2, 0, czasOp2, nrMOp2);
            czasZakonczeniaOperacjiJeden = czasOp1;
      }

      bool czyMoznaRozpoczacOperacjeDwa(int czasRozpoczeciaOperacjiDwa) {
            if(czasRozpoczeciaOperacjiDwa < czasZakonczeniaOperacjiJeden)
                  return false;
            return true;
      }
};


bool kolidujeZPrzerwa(Operacja operacja, Przerwa przerwa);
#include <iostream>
struct Maszyna {
      short nrMaszyny;
      std::vector<Operacja> operacje;
      std::vector<Przerwa> przerwy;
      int czasMaszyny = 0;
      int calkowityCzas = 0;
      std::vector<bool> czyZakonczonoOperacjeNumerJeden;

      Maszyna() {};
      Maszyna(short nrM, std::vector<Operacja> op, std::vector<Przerwa> prz, int czasM) : nrMaszyny(nrM), operacje(op),
      przerwy(prz), czasMaszyny(czasM) {};

      void wypiszPrzerwyIOperacje() {
            for(int i = 0; i < przerwy.size(); i++) {
                  std::cout << przerwy[i].czasRozpoczecia << " | " << przerwy[i].czasRozpoczecia + przerwy[i].czasTrwania << std::endl;
            }

            for(int i = 0; i < operacje.size(); i++) {
                  std::cout << operacje[i].czasRozpoczecia << " | " << operacje[i].czasRozpoczecia + operacje[i].czasOperacji << std::endl;
            }
      }

      void rozlozOperacjeNaMaszynie() {
            int nrPrzerwy = 0;
            czasMaszyny = 0;
            for(int i = 0; i < czyZakonczonoOperacjeNumerJeden.size(); i++)
                  czyZakonczonoOperacjeNumerJeden[i] = false;



            for(int i = 0; i < operacje.size(); i++) {
                  operacje[i].czasRozpoczecia = czasMaszyny;

                  while(nrPrzerwy < przerwy.size() && kolidujeZPrzerwa(operacje[i], przerwy[nrPrzerwy])) {
                        czasMaszyny = przerwy[nrPrzerwy].czasRozpoczecia + przerwy[nrPrzerwy].czasTrwania;
                        nrPrzerwy++;
                        operacje[i].czasRozpoczecia = czasMaszyny;
                  }
                  czasMaszyny += operacje[i].czasOperacji;
            }
            calkowityCzas = czasMaszyny;
      }
};

struct Osobnik {
      Maszyna maszyna1;
      Maszyna maszyna2;
      int czasUszeregowania = 0;

      Osobnik() {};
      Osobnik(std::vector<Gen> uszeregowanie, std::vector<Przerwa> przerwyM1, std::vector<Przerwa> przerwyM2){
            maszyna1.przerwy = przerwyM1;
            maszyna2.przerwy = przerwyM2;

            dodajOperacjeDoMaszyn(uszeregowanie);
      }

      void dodajOperacjeDoMaszyn(std::vector<Gen> uszeregowanie) {
            int czas1 = 0;
            int czas2 = 0;
            for(int i = 0; i < uszeregowanie.size(); i++) {
                  if(uszeregowanie[i].operacje[0].nrMaszyny == 1) {
                        maszyna1.operacje.push_back(uszeregowanie[i].operacje[0]);
                        czas1 += uszeregowanie[i].operacje[0].czasOperacji;
                        maszyna2.operacje.push_back(uszeregowanie[i].operacje[1]);
                        czas2 += uszeregowanie[i].operacje[1].czasOperacji;
                  }
                  else {
                        maszyna1.operacje.push_back(uszeregowanie[i].operacje[1]);
                        czas1 += uszeregowanie[i].operacje[1].czasOperacji;
                        maszyna2.operacje.push_back(uszeregowanie[i].operacje[0]);
                        czas2 += uszeregowanie[i].operacje[0].czasOperacji;
                  }
            }
      }

      int zwrocCzasUszeregowania() {
            czasUszeregowania = std::max(maszyna1.calkowityCzas, maszyna2.calkowityCzas);
            return czasUszeregowania;
      }

      void rozlozOperacje() {
            int tab[maszyna1.operacje.size()];  // czas zakonczenia operacji numer 1 dla zadania [x]
            for(int i = 0; i < maszyna1.operacje.size(); i++)
                  tab[i] = 9999999;
            int nrPrzerwyM1 = 0, nrPrzerwyM2 = 0, czasMaszyny1 = 0, czasMaszyny2 = 0;

            for(int i = 0; i < maszyna1.operacje.size(); i++) {
                  maszyna1.operacje[i].czasRozpoczecia = czasMaszyny1;
                  if(maszyna1.operacje[i].nrOperacji == 2) {
                        if(tab[maszyna1.operacje[i].nrZadania] < maszyna1.operacje[i].czasRozpoczecia)
                              ;
                        else {
                              int j = i+1;
                              while(j < maszyna1.operacje.size() && maszyna1.operacje[j].nrOperacji == 2 && tab[maszyna1.operacje[j].nrZadania] > maszyna1.operacje[j].czasRozpoczecia) {
                                    j++;
                              }

                              if(j == maszyna1.operacje.size()) {
                                    int k = i+1;
                                    while(k < j) {
                                          k++;
                                          if(maszyna1.operacje[k].nrOperacji == 1) {
                                                Operacja temp = maszyna1.operacje[k];
                                                maszyna1.operacje[k] = maszyna1.operacje[i];
                                                maszyna1.operacje[i] = temp;
                                                break;
                                          }
                                    }
                              }
                              else {
                                    Operacja temp = maszyna1.operacje[j];
                                    maszyna1.operacje[j] = maszyna1.operacje[i];
                                    maszyna1.operacje[i] = temp;
                              }
                        }
                  }
                  maszyna1.operacje[i].czasRozpoczecia = czasMaszyny1;

                  while(nrPrzerwyM1 < maszyna1.przerwy.size() && kolidujeZPrzerwa(maszyna1.operacje[i], maszyna1.przerwy[nrPrzerwyM1])) {
                        czasMaszyny1 = maszyna1.przerwy[nrPrzerwyM1].czasRozpoczecia + maszyna1.przerwy[nrPrzerwyM1].czasTrwania;
                        nrPrzerwyM1++;
                        maszyna1.operacje[i].czasRozpoczecia = czasMaszyny1;
                  }

                  maszyna2.operacje[i].czasRozpoczecia = czasMaszyny2;
                  if(maszyna2.operacje[i].nrOperacji == 2) {
                        if(tab[maszyna2.operacje[i].nrZadania] < maszyna2.operacje[i].czasRozpoczecia)
                              ;
                        else {
                              int j = i+1;
                              while(j < maszyna2.operacje.size() && maszyna2.operacje[j].nrOperacji == 2 && tab[maszyna2.operacje[j].nrZadania] > maszyna2.operacje[i].czasRozpoczecia) {
                                    j++;
                              }

                              if(j == maszyna2.operacje.size()) {
                                    int k = i+1;
                                    while(k < j) {
                                          k++;
                                          if(maszyna2.operacje[k].nrOperacji == 1) {
                                                Operacja temp = maszyna2.operacje[k];
                                                maszyna2.operacje[k] = maszyna2.operacje[i];
                                                maszyna2.operacje[i] = temp;
                                                break;
                                          }
                                    }
                              }
                              else {
                                    Operacja temp = maszyna2.operacje[j];
                                    maszyna2.operacje[j] = maszyna2.operacje[i];
                                    maszyna2.operacje[i] = temp;
                              }
                        }
                  }
                  maszyna2.operacje[i].czasRozpoczecia = czasMaszyny2;

                  while(nrPrzerwyM2 < maszyna2.przerwy.size() && kolidujeZPrzerwa(maszyna2.operacje[i], maszyna2.przerwy[nrPrzerwyM2])) {
                        czasMaszyny2 = maszyna2.przerwy[nrPrzerwyM2].czasRozpoczecia + maszyna2.przerwy[nrPrzerwyM2].czasTrwania;
                        nrPrzerwyM2++;
                        maszyna2.operacje[i].czasRozpoczecia = czasMaszyny2;
                  }
                  czasMaszyny1 += maszyna1.operacje[i].czasOperacji;
                  czasMaszyny2 += maszyna2.operacje[i].czasOperacji;
                  if(maszyna1.operacje[i].nrOperacji == 1)
                        tab[maszyna1.operacje[i].nrZadania] = czasMaszyny1;
                  if(maszyna2.operacje[i].nrOperacji == 1)
                        tab[maszyna2.operacje[i].nrZadania] = czasMaszyny2;
            }
            maszyna1.calkowityCzas = czasMaszyny1;
            maszyna2.calkowityCzas = czasMaszyny2;
      }

      void rozlozOperacjeNaMaszynach() {
            rozlozOperacje();
            zwrocCzasUszeregowania();
      }
};

Przerwa przerwaZPliku(std::string line);
Gen genZPliku(std::string line);
std::vector<Przerwa> ustalPrzerwy(std::string plikInstancji);
std::vector<Osobnik> stworzPopulacje(std::vector<Gen> usz, std::vector<Przerwa> & przerwy);
std::vector<Gen> wczytajGenyZPliku(std::string plikInstancji);
void zapiszDoPliku(std::string wynikInstancji, std::vector<Osobnik> & populacja, int czasPoczatkowy);

void funkcjaNaprawiajaca(std::vector<Przerwa> & przerwy, std::vector<Osobnik> & populacja);

// selekcja
void selekcjaPopulacji(std::vector<Osobnik> & populacja);

// krzyzuj maszyny
void krzyzuj(std::vector<Osobnik> & populacja);
void naprawPopulacje(std::vector<Osobnik> & populacja, std::vector<Gen> & uszeregowanie);
void naprawOsobnika(Osobnik & o, std::vector<Gen> & uszeregowanie);

// mutacja
void zamienGeny(struct Osobnik & osobnik, int a, int b, int c);
void mutuj(int mutacja, std::vector<Osobnik> & populacja);

// komparator

struct Comparator
{
    inline bool operator() (const Osobnik &o1, const Osobnik &o2)
    {
        return o1.czasUszeregowania < o2.czasUszeregowania;
    }
};

#endif // ALGORYTM_H_INCLUDED
