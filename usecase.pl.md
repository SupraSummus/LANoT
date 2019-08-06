Ale o co kaman?
===============

System orchestracji urządzeń IoT.

Ustandaryzowanie roli pojedynczego urządzenia do takiej postaci, że zarządzanie całą chmarą jest tworzeniem jednej konfiguracji.

Zapewnienie taniego i prostego systemu do tworzenia "inteligentnych domów".

Przpadek użycia JR
------------------

Chciałbym:
 * Zrobić termometr, powiesić go na drzewie w ogórdku i mierzyć temperaturę. Wyswietlać ją na wyświetlaczu zamontowanym w przedpokoju.
 * Oprócz tego dobrze by zbierać pomiary i rysować sobie wykres (dostępny przez http z mojego domowego serwera, który bym sam oprogramował).
 * Mierzyć temperaturę wody w boilerze i rysować wykres - do celów debuggingu, bo boiler jakoś dziwnie działa i woda czasem jest zimna.
 * Móc włączyć światło za pomocą komputera. Potem napisać reguły crona, które by mi to światło same włączały i wyłączały o określonych porach.
 * Otworzyć furtkę nie ruszajac sie z kanapy - a przycisk otwierania jest przy drzwiach wejsciowych.
 * Mieć pilota do odtwarzacza muzyki (radia internetowego) podłączonego do kolumn. Jest to Raspberry PI + mpd, a zatem nie mam żadnego pilota jak np do odtwarzacza DVD lub telewizora. Poza tym dobrze by było, gdyby pilot działał w całym domu, a nie tylko po optycznej.

Nie chcę:
 * kupować gotowych rozwiązań (po pierwsze ich nie ma) - bo nigdy nie spełnią 100% potrzeb i nie są dostatecznie elastyczne
 * babrać się w niskopoziomowe programowanie mikrokontrolerów w C
 * nie móc babrać sie w C - bo czasami mam na to ochotę albo trzeba sie pobabrać, bo inaczej się nie da
 * chodzić z komputerem i programatorem do miejsca w którym jest powieszony termometr, żeby zmienić częstotliwość pomiarów
 * zmieniać baterii co tydzień
 * wysyłać moich danych pomiarowych "w chmurę"
 * mieć poczucia, że moje działania mogą być podsłuchiwane
 * konfigurować sieci
 * nie móc konfigurować sieci
 * wydawać fortunę na pojedyncze urządzenie
 * mieć problemy z zasięgiem lub ciągać kabli

Projekt
-------

Urządzenia
----------

System on chip nRF52840 - ma procek, RAM, flash, radio (BT, ANT, ...), peryferia (ADC, UART, I2C, ...)

przykładowy moduł 10$/sztukę http://www.fanstel.com/bt840f-nrf52840-ble-5-module-secure-iot-802154-thread-zigbee/

tutaj ktoś ma startup na tym https://www.particle.io/mesh/


