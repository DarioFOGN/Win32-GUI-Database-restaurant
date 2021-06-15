#ifndef MYSTRUCT_H
#define MYSTRUCT_H

typedef struct ristorante{
    char nome[20];
    int codice_id;
    char localita[15];
    char tradizione[20];
    float fasciadiprezzo;
    float media_voti;
    int p_prenotabili;
    char chef[12];
}ristorante;

typedef struct recensione{
    char username[20];
    int id_rest;
    int val;
    struct recensione *next;
}recensione;


typedef struct prenotazione{
    char username[20];
    int id_rest;
    int giorno, mese, anno;
    int posti_prenotati;
    struct prenotazione *next;
}prenotazione;


typedef struct utente{
    char username[20];
    char email[30];
    int  n_anno;//anno di nascita
    char tradizione[20];
    float fasciadiprezzo;
    float media_voti;
    struct prenotazione *p_prenotabili;
    struct recensione *valutazioni;
}utente;

#endif // HASHTABLE_H_INCLUDED

 