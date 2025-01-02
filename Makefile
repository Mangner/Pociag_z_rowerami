all: Kierownik_Pociagu Zawiadowca_Stacji Pasazerowie Pasazer


Kierownik_Pociagu: 
	gcc Kierownik_Pociagu.c My_Library/message_queue_operations.c My_Library/shared_memory_operations.c My_Library/semafor_operations.c -o Kierownik_Pociagu -lpthread

Zawiadowca_Stacji:
	gcc Zawiadowca_Stacji.c My_Library/message_queue_operations.c My_Library/shared_memory_operations.c My_Library/semafor_operations.c -o Zawiadowca_Stacji -lpthread

Pasazerowie:
	gcc Pasazerowie.c My_Library/message_queue_operations.c My_Library/shared_memory_operations.c My_Library/semafor_operations.c -o Pasazerowie -lpthread

Pasazer:
	gcc Pasazer.c My_Library/message_queue_operations.c My_Library/shared_memory_operations.c My_Library/semafor_operations.c -o Pasazer -lpthread 

clean:
	rm -r Kierownik_Pociagu Zawiadowca_Stacji Pasazerowie Pasazer 
	ipcrm -a