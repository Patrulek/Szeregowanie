//============================================
// PROJEKT Z SZEREGOWANIA ZADAÑ 12-01-2015
//============================================
// Opis projektu:
//============================================
// 2. Uszeregowanie n zadañ (minimum 200) na dwóch maszynach (Jobshop), z przerwami konserwuj¹cymi na
// obu maszynach maj¹cymi ustalony wczeœniej czas trwania oraz czas rozpoczêcia*. Liczba przerw w instancji - zmienna**.
// Operacje niewznawialne. Minimalizowaæ czas zakoñczenia ca³ego uszeregowania.
//============================================
// *losowanie czasu rozpoczêcia przerwy: w przedziale od zera do 0.45 * S, gdzie S to suma czasów trwania
// wszystkich operacji ze wszystkich zadañ w ramach danej instancji problemu.
// Przerwy (w ramach tej samej maszyny) nie mog¹ na siebie nachodziæ.
//
// **liczba przerw - parametr generatora instancji, np. 0.1 * n, 0.15 * n, lub jakkolwiek ustalany przez generator instancji
//
// Minimalny czas trwania operacji w ka¿dym zadaniu = 20 jednostek czasu, maksymalna = 100 jednostek.
//============================================
// Format danych wejœciowych (jeœli dany element w problemie nie wystêpuje jako ustalony, w ogóle
// jego dane siê nie pojawiaj¹ w pliku wejœciowym)
//============================================
// **** NR INSTANCJI PROBLEMU ****
// liczba_zadañ
// czas_operacji1_1; czas_operacji1_2; nr_maszyny_dla_op1_1; nr_maszyny_dla_op1_2; waga1_1; waga1_2;
// czas_operacji2_1; czas_operacji2_2; nr_maszyny_dla_op2_1; nr_maszyny_dla_op2_2; waga2_1; waga2_2;
// <itd. a¿ do ostatniego zadania z dwoma operacjami>
// nr_przerwy; nr_maszyny; czas trwania przerwy; czas_startu_przerwy
// <itd. a¿ do wszystkich przerw – przy czym nie dotyczy to problemu 3>
//*** EOF ***
//============================================
// Podstawowy format pliku wyjœciowego algorytmu rozwi¹zuj¹cego dany problem:
//============================================
// **** NR INSTANCJI PROBLEMU ****
// maksymalny_czas_uszeregowania ; czas_pocz¹tkowy (lub inne kryterium optymalizacyjne w zal. od problemu)
// Nastêpnie, w dwóch linijkach, wymieniamy uszeregowane elementy na pierwszej maszynie (linia I pliku) oraz na drugiej maszynie (linia II pliku) wg schematu:
// M1: id/nazwa_elementu, par1, par2, par3; id/nazwa_elementu, par1, par2, par3; itd.
// M2: id/nazwa_elementu, par1, par2, par3; id/nazwa_elementu, par1, par2, par3; itd.
// ³¹czna_liczba_przerw_konserwuj¹cych_M1, ich_sumaryczny_czas_trwania_na_M1
// ³¹czna_liczba_przerw_konserwuj¹cych_M2, ich_sumaryczny_czas_trwania_na_M2
// ³¹czna_liczba_przerw_typu_idle_M1, ich_sumaryczny_czas_trwania_na_M1
// ³¹czna_liczba_przerw_typu_idle_M2, ich_sumaryczny_czas_trwania_na_M2
// *** EOF ***
//============================================
// id/nazwa_elementu:
// - operacja (oznaczenie: np. o1_z1, o2_16, o1_z99, patrz ni¿ej)
// -par1: czas startu uszeregowania dla operacji
// -par2: d³ugoœæ operacji wg instancji
// -par3: d³ugoœæ rzeczywista dla operacji w uszeregowaniu (dla problemu I i III)
// dla problemu V: par3 to waga operacji, dla problemów II i IV: brak par3
//============================================
// - przerwa konserwuj¹ca (oznaczenia: maint1_M1, maint2_M1, maint8_M2, itd.)
// -par1: czas startu przerwy
// -par2: czas trwania przerwy
// -par3: nie wystêpuje
//============================================
// - przerwa gdzie nic siê nie dzieje (ale nie konserwuj¹ca) (oznaczenie: idle1_M1, idle21_M2, itd.)
// -par1: czas startu przerwy w szeregowaniu
// -par2: czas trwania
// -par3: nie wystêpuje
//============================================
// Objaœnienia dodatkowe:
// id_operacji/zadania oznacza np.:
// o1_z1 – 1 operacja 1 zadania
// o1_z2 – 1 operacja 2 zadania
// o2_z17 – 2 operacja 17 zadania, itd.
// Zaokr¹glanie: w górê, czasy wszystkiego: liczby ca³kowite.
// Nazwy plików musz¹ umo¿liwiaæ ³atwe rozpoznanie który plik wyjœciowy zawiera rozwi¹zanie dla którego
// pliku instancji – czyli np. numer/seria/id instancji powinny byæ w ramach nazw plików usystematyzowane.
//============================================


#include "generator.h"
#include "algorytm.h"
#include "parametry.h"

#include <iostream>
#include <ctime>
#include <fstream>

std::vector<Przerwa> przerwy;
std::vector<Osobnik> populacja;
std::vector<Gen> uszeregowanie;

int main() {
      generujInstancje();

      std::string nazwa = "instancja";
      std::string nr = intToString(NUMER_INSTANCJI);
      std::string in = nazwa + nr + ".txt";
      std::string out = nazwa + nr + "out.txt";

      przerwy = ustalPrzerwy(in);
      uszeregowanie = wczytajGenyZPliku(in);
      populacja = stworzPopulacje(uszeregowanie, przerwy);

      if(przerwy.empty() || populacja.empty() || uszeregowanie.empty()) {
            std::cout << "Niepoprawna inicjalizacja danych!\n";
            return -1;
      }

      int czasPoczatkowy = 200000;
      int nrTestu = 1;
      double czas[5][15];
      std::fstream f("res.txt", std::fstream::out | std::fstream::trunc);
      for(int x = 0; x < 5; x++) {
            int licznik = 0;
            int ktoryPomiar = 0;
            int procent = 0;
            clock_t time=clock();
            for(int t = 0; t < LICZBA_ITERACJI; t++) {
                  mutuj(WSPOLCZYNNIK_MUTACJI, populacja);
                  krzyzuj(populacja);
                  naprawPopulacje(populacja, uszeregowanie);
                  selekcjaPopulacji(populacja);
                  int min = 200000;
                  for(int i = 0; i < populacja.size(); i++) {
                        min = std::min(min, populacja[i].czasUszeregowania);
                  }

                  if(t == 0) czasPoczatkowy = min;
                  f << min << std::endl;

                 licznik++;
                 if(licznik % (LICZBA_ITERACJI / 10) == 0) {
                        procent += 10;
                        std::cout << "Wykonano juz " << procent << "% wszystkich iteracji\n";
                 }

                 if(t % 1000 == 999) {
                        czas[x][ktoryPomiar] = (double(clock() - time))/CLOCKS_PER_SEC;
                        ktoryPomiar++;
                 }
            }

           // f << (float(clock() - time))/CLOCKS_PER_SEC << "\n";
      }

      for(int a = 0; a < 15; a++) {
            for(int b = 0; b < 5; b++) {
                  f << czas[b][a] << "\n";
            }
      }

      zapiszDoPliku(out, populacja, czasPoczatkowy);

      f.close();
      return 0;
}
