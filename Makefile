all: Kierwonik_Pociagu


Kierwonik_Pociagu: Kierwonik_Pociagu.c
	gcc Kierwonik_Pociagu.c -o Kierwonik_Pociagu -lpthread -message_queue

clean:
	rm -r Kierwonik_Pociagu