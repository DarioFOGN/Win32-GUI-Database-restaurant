#ifndef HASHTABLE_H_INCLUDED
#define HASHTABLE_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <ctype.h>
#include "mystruct.h"


//predichiarazione delle funzioni
utente *hash_Table_LookUtente(char *name);
ristorante *hash_Table_LookRistorante(int codice_id);
int hash_Table_IndexRI(int codice_id);
int hash_Table_IndexUT(char *name);

//prende le hash dall main
extern utente *hash_table_utenti[];
extern ristorante *hash_table_ristoranti[];
extern HWND hRecTextb;

#define MAX_DIM 1000
#define MAX_NAME 20
#define TABLE_SIZE 1000

void toLower(char* s) {
  for(char *p=s; *p; p++) *p=tolower(*p);
}

unsigned int hashR(int codice_id)//per la nostra nostra hash table ci serve passare il codice_id, come chiave
//di riferimento
{
    codice_id = ((codice_id >> 16) ^ codice_id) * 0x45d9f3b;
    codice_id = ((codice_id >> 16) ^ codice_id) * 0x45d9f3b;
    codice_id = (codice_id >> 16) ^ codice_id;
    codice_id = (codice_id) % TABLE_SIZE;
    return codice_id;
}

unsigned int hashU(char *name)//per la nostra nostra hash table ci serve passare il nome, come chiave
//di riferimento
{
    int lenght = strnlen(name, MAX_NAME);
    unsigned int hash_value = 0;
    for(int i = 0; i < lenght; i++){
        hash_value += name[i];//essendo name un puntatore quindi un numero possiamo utilizzarlo come riferimento per fare la
        //nostra hash function
        hash_value = (hash_value * name[i]) % TABLE_SIZE;//per avere numeri ancora più randomici successivamente lo andiamo a
        //moltiplicare come (di nuovo) name[i] con modulazione table_size perchè vogliamo avere valori che rientrano la nostra
        //hash table
    }
    return hash_value;
}

void init_hash_table(){//inizializazzione della nostra hash table
    for(int i = 0; i<TABLE_SIZE; i++)
    {
        hash_table_utenti[i] = NULL;
        hash_table_ristoranti[i] = NULL;
    }
}

void print_hashUtenti(){

    printf("--------------UTENTI-------------------\n");
    for (int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_utenti[i]!=NULL)
            printf("[%i]\t %s, %s, %d, %s, %0.1f, %0.1f\n",i, hash_table_utenti[i]->username, hash_table_utenti[i]->email, hash_table_utenti[i]->n_anno, hash_table_utenti[i]->tradizione, 
            hash_table_utenti[i]->fasciadiprezzo, hash_table_utenti[i]->media_voti);
    }
    printf("---------------------------------------\n");
}

void print_hashRisto(){
    printf("--------------RISTORANTI---------------\n");
    for (int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_ristoranti[i]!=NULL)
            printf("[%i]\t %s, %d, %s, %s, %0.1f, %0.1f, %d, %s\n",i, hash_table_ristoranti[i]->nome, hash_table_ristoranti[i]->codice_id, hash_table_ristoranti[i]->localita,
            hash_table_ristoranti[i]->tradizione, hash_table_ristoranti[i]->fasciadiprezzo, hash_table_ristoranti[i]->media_voti, hash_table_ristoranti[i]->p_prenotabili,
            hash_table_ristoranti[i]->chef);
    }
    printf("---------------------------------------\n");
}


void print_table_all(){
    printf("Start\n");
    for (int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_utenti[i]==NULL)
        printf("\t%i\t---\n", i);
        else
        printf("\t%i\t%s\n",i, hash_table_utenti[i]->username);
    }
    printf("End\n");
    printf("Start\n");
    for (int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_ristoranti[i]==NULL)
        printf("\t%i\t---\n", i);
        else
        printf("\t%i\t%d\n",i, hash_table_ristoranti[i]->codice_id);
    }
    printf("End\n");
}

int hash_table_insertR(ristorante *p){

    struct ristorante *tmp = (struct ristorante*) malloc(sizeof(struct ristorante));

    strcpy(tmp->nome, p->nome);
    tmp->codice_id = p->codice_id;
    strcpy(tmp->localita, p->localita);
    strcpy(tmp->tradizione, p->tradizione);
    tmp->fasciadiprezzo = p->fasciadiprezzo;
    tmp->media_voti = p->media_voti;
    tmp->p_prenotabili = p->p_prenotabili;
    strcpy(tmp->chef, p->chef);
    
    int hashIndex = hashR(tmp->codice_id);
    if(hash_table_ristoranti[hashIndex] != NULL && hash_table_ristoranti[hashIndex]->codice_id == tmp->codice_id){
        //se l'utente inserirà un ristorante già inserito ritorniamo 1 (per il main) e non duplichiamo lo stesso dato nella hash
        return 1;
    }
    else{
        //move in array until an empty or deleted cell
        while(hash_table_ristoranti[hashIndex] != NULL){
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= TABLE_SIZE;
        }
        hash_table_ristoranti[hashIndex] = tmp;
    }
   
}

int hash_table_insertU(utente *p){

    struct utente *tmp = (struct utente*) malloc(sizeof(struct utente));
    //per semplicità facciamo la stringa minuscola così da evitare problemi con la ricerca
    toLower(p->username);
    strcpy(tmp->username, p->username);
    strcpy(tmp->email, p->email);
    tmp->n_anno = p->n_anno;
    strcpy(tmp->tradizione, p->tradizione);
    tmp->fasciadiprezzo = p->fasciadiprezzo;
    tmp->media_voti = p->media_voti;
    tmp->p_prenotabili = NULL;
    tmp->valutazioni = NULL;

    int hashIndex = hashU(tmp->username);
    if(hash_table_utenti[hashIndex] != NULL && strcmp(hash_table_utenti[hashIndex]->username, tmp->username)==0){
        //se l'utente inserirà un utente già inserito ritorniamo 1 (per il main) e non duplichiamo lo stesso dato nella hash
        return 1;
    }
    else{
        //move in array until an empty or deleted cell
        while(hash_table_utenti[hashIndex] != NULL && hash_table_utenti[hashIndex]->username != "") {
        //go to next cell
        ++hashIndex;

        //wrap around the table
        hashIndex %= TABLE_SIZE;
        }
        hash_table_utenti[hashIndex] = tmp;
    } 
}

int hash_table_ModU(utente *p){

    //per semplicità facciamo la stringa minuscola così da evitare problemi con la ricerca
    toLower(p->username);
    utente *tmp = hash_Table_LookUtente(p->username);

    strcpy(tmp->username, p->username);
    strcpy(tmp->email, p->email);
    tmp->n_anno = p->n_anno;
    strcpy(tmp->tradizione, p->tradizione);
    tmp->fasciadiprezzo = p->fasciadiprezzo;
}

int hash_table_DelU(char *name){
    //per semplicità facciamo la stringa minuscola così da evitare problemi con la ricerca
    toLower(name);

    int index = hash_Table_IndexUT(name);
    if(index <= -1)
    return -1;
    free(hash_table_utenti[index]);
    hash_table_utenti[index] = NULL;
}

int hash_table_DelR(int codiceid){
    int index = hash_Table_IndexRI(codiceid);
    free(hash_table_ristoranti[index]);
    hash_table_ristoranti[index] = NULL;
}

int hash_table_ModR(ristorante *p){

    
    //per semplicità facciamo la stringa minuscola così da evitare problemi con la ricerca
    ristorante *tmp = hash_Table_LookRistorante(p->codice_id);

    strcpy(tmp->nome, p->nome); 
    strcpy(tmp->localita, p->localita); 
    strcpy(tmp->tradizione, p->tradizione);
    strcpy(tmp->chef, p->chef); 
    tmp->codice_id = p->codice_id;
    tmp->p_prenotabili = p->p_prenotabili;
    tmp->fasciadiprezzo = p->fasciadiprezzo;
    tmp->media_voti = p->media_voti;
}

//ritorna l'utente cercato
utente *hash_Table_LookUtente(char *name){
    toLower(name);
    int index = hashU(name);
    if(hash_table_utenti[index] != NULL && strncmp(hash_table_utenti[index]->username, name, TABLE_SIZE)==0)
        return hash_table_utenti[index];
    else
        return NULL;
}

//ritorna l'index del utente in questione utile per la cancellazione di un utente
int hash_Table_IndexUT(char *name){
    toLower(name);
    int index = hashU(name);
    if(hash_table_utenti[index] != NULL && strncmp(hash_table_utenti[index]->username, name, TABLE_SIZE)==0)
        return index;
    else
        return -1;
}

//ritorna il  ristorante cercato
ristorante *hash_Table_LookRistorante(int codice_id){
    int index = hashR(codice_id);
    if(hash_table_ristoranti[index] != NULL && codice_id==hash_table_ristoranti[index]->codice_id)
        return hash_table_ristoranti[index];
    else
        return NULL;
}

//ritorna l'index del ristorante in questione utile per la cancellazione di un rist
int hash_Table_IndexRI(int codice_id){
    int index = hashR(codice_id);
    if(hash_table_ristoranti[index] != NULL && codice_id==hash_table_ristoranti[index]->codice_id)
        return index;
    else
        return -1;
}

//ricerca ristorante in base alla media e lo stampa
void McercaRI(float media){
    char str[10000], str2[100];
    strcpy(str, "I Ristoranti TROVATI SONO:\r\n");
    for(int i = 0; i<TABLE_SIZE; i++){
            if(hash_table_ristoranti[i]!=NULL && hash_table_ristoranti[i]->media_voti>media-1 &&hash_table_ristoranti[i]->media_voti<media+1){
                strcat(str, "NOME: ");
                strcat(str, hash_table_ristoranti[i]->nome);
                strcat(str, " ");
                itoa(hash_table_ristoranti[i]->codice_id, str2, 10);
                strcat(str, "\r\nCODICE ID:: ");
                strcat(str, str2);
                itoa(hash_table_ristoranti[i]->fasciadiprezzo, str2, 10);
                strcat(str, "\r\nFASCIA DI PREZZO: ");
                strcat(str, str2);
                strcat(str, "\r\n\n");
            }
    }
    SetWindowText(hRecTextb,str);   
}
//ricerca ristorante in base alla località e lo stampa
void LcercaRI(char localita[]){
    char str[10000], str2[100];
    strcpy(str, "I Ristoranti TROVATI SONO:\r\n");
    for(int i = 0; i<TABLE_SIZE; i++){
            if(hash_table_ristoranti[i]!=NULL){
                toLower(hash_table_ristoranti[i]->localita);
                toLower(localita);
                if(strcmp(hash_table_ristoranti[i]->localita,localita)==0){
                    strcat(str, "NOME: ");
                    strcat(str, hash_table_ristoranti[i]->nome);
                    itoa(hash_table_ristoranti[i]->codice_id, str2, 10);
                    strcat(str, "\r\nCODICE ID: ");
                    strcat(str, str2);
                    itoa(hash_table_ristoranti[i]->fasciadiprezzo, str2, 10);
                    strcat(str, "\r\nFASCIA DI PREZZO: ");
                    strcat(str, str2);
                    strcat(str, "\r\n\n");
                }
            }   
    }
    SetWindowText(hRecTextb,str);    
}
//ricerca ristorante in base alla località e lo stampa
void fcercaRI(int fascia){
    char str[10000], str2[100];
    strcpy(str, "I Ristoranti TROVATI SONO:\r\n");
    for(int i = 0; i<TABLE_SIZE; i++){
            if(hash_table_ristoranti[i]!=NULL && hash_table_ristoranti[i]->fasciadiprezzo < fascia){
                strcat(str, "NOME: ");
                strcat(str, hash_table_ristoranti[i]->nome);
                strcat(str, " ");
                itoa(hash_table_ristoranti[i]->codice_id, str2, 10);
                strcat(str, "\r\nCODICE ID: ");
                strcat(str, str2);
                itoa(hash_table_ristoranti[i]->fasciadiprezzo, str2, 10);
                strcat(str, "\r\nFASCIA DI PREZZO: ");
                strcat(str, str2);
                strcat(str, "\r\n\n");
            }
    } 
    SetWindowText(hRecTextb,str);   
}
#endif // HASHTABLE_H_INCLUDED
