//============================================
// PROJEKT Z SZEREGOWANIA ZADA� 12-01-2015
//============================================
// Opis projektu:
//============================================
// 2. Uszeregowanie n zada� (minimum 200) na dw�ch maszynach (Jobshop), z przerwami konserwuj�cymi na
// obu maszynach maj�cymi ustalony wcze�niej czas trwania oraz czas rozpocz�cia*. Liczba przerw w instancji - zmienna**.
// Operacje niewznawialne. Minimalizowa� czas zako�czenia ca�ego uszeregowania.
//============================================
// *losowanie czasu rozpocz�cia przerwy: w przedziale od zera do 0.45 * S, gdzie S to suma czas�w trwania
// wszystkich operacji ze wszystkich zada� w ramach danej instancji problemu.
// Przerwy (w ramach tej samej maszyny) nie mog� na siebie nachodzi�.
//
// **liczba przerw - parametr generatora instancji, np. 0.1 * n, 0.15 * n, lub jakkolwiek ustalany przez generator instancji
//
// Minimalny czas trwania operacji w ka�dym zadaniu = 20 jednostek czasu, maksymalna = 100 jednostek.
//============================================
// Format danych wej�ciowych (je�li dany element w problemie nie wyst�puje jako ustalony, w og�le
// jego dane si� nie pojawiaj� w pliku wej�ciowym)
//============================================
// **** NR INSTANCJI PROBLEMU ****
// liczba_zada�
// czas_operacji1_1; czas_operacji1_2; nr_maszyny_dla_op1_1; nr_maszyny_dla_op1_2; waga1_1; waga1_2;
// czas_operacji2_1; czas_operacji2_2; nr_maszyny_dla_op2_1; nr_maszyny_dla_op2_2; waga2_1; waga2_2;
// <itd. a� do ostatniego zadania z dwoma operacjami>
// nr_przerwy; nr_maszyny; czas trwania przerwy; czas_startu_przerwy
// <itd. a� do wszystkich przerw � przy czym nie dotyczy to problemu 3>
//*** EOF ***
//============================================
// Podstawowy format pliku wyj�ciowego algorytmu rozwi�zuj�cego dany problem:
//============================================
// **** NR INSTANCJI PROBLEMU ****
// maksymalny_czas_uszeregowania ; czas_pocz�tkowy (lub inne kryterium optymalizacyjne w zal. od problemu)
// Nast�pnie, w dw�ch linijkach, wymieniamy uszeregowane elementy na pierwszej maszynie (linia I pliku) oraz na drugiej maszynie (linia II pliku) wg schematu:
// M1: id/nazwa_elementu, par1, par2, par3; id/nazwa_elementu, par1, par2, par3; itd.
// M2: id/nazwa_elementu, par1, par2, par3; id/nazwa_elementu, par1, par2, par3; itd.
// ��czna_liczba_przerw_konserwuj�cych_M1, ich_sumaryczny_czas_trwania_na_M1
// ��czna_liczba_przerw_konserwuj�cych_M2, ich_sumaryczny_czas_trwania_na_M2
// ��czna_liczba_przerw_typu_idle_M1, ich_sumaryczny_czas_trwania_na_M1
// ��czna_liczba_przerw_typu_idle_M2, ich_sumaryczny_czas_trwania_na_M2
// *** EOF ***
//============================================
// id/nazwa_elementu:
// - operacja (oznaczenie: np. o1_z1, o2_16, o1_z99, patrz ni�ej)
// -par1: czas startu uszeregowania dla operacji
// -par2: d�ugo�� operacji wg instancji
// -par3: d�ugo�� rzeczywista dla operacji w uszeregowaniu (dla problemu I i III)
// dla problemu V: par3 to waga operacji, dla problem�w II i IV: brak par3
//============================================
// - przerwa konserwuj�ca (oznaczenia: maint1_M1, maint2_M1, maint8_M2, itd.)
// -par1: czas startu przerwy
// -par2: czas trwania przerwy
// -par3: nie wyst�puje
//============================================
// - przerwa gdzie nic si� nie dzieje (ale nie konserwuj�ca) (oznaczenie: idle1_M1, idle21_M2, itd.)
// -par1: czas startu przerwy w szeregowaniu
// -par2: czas trwania
// -par3: nie wyst�puje
//============================================
// Obja�nienia dodatkowe:
// id_operacji/zadania oznacza np.:
// o1_z1 � 1 operacja 1 zadania
// o1_z2 � 1 operacja 2 zadania
// o2_z17 � 2 operacja 17 zadania, itd.
// Zaokr�glanie: w g�r�, czasy wszystkiego: liczby ca�kowite.
// Nazwy plik�w musz� umo�liwia� �atwe rozpoznanie kt�ry plik wyj�ciowy zawiera rozwi�zanie dla kt�rego
// pliku instancji � czyli np. numer/seria/id instancji powinny by� w ramach nazw plik�w usystematyzowane.
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
