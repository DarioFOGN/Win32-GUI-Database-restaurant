#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED

#include <stdbool.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mystruct.h"
#include "hashtable.h"

extern struct tm getime();

//Inserisce le Recensioni in testa
void insertRec(struct utente *utente,char nome[20],int id_rest, int valore) {
   //crea un link allo heap per la recensione
   struct recensione *tmp = (struct recensione*) malloc(sizeof(struct recensione));//(struct recensione*) puntatore della malloc della classe recensione

   strcpy(tmp->username, nome);
   tmp->id_rest = id_rest;
   tmp->val = valore;
   tmp->next = NULL;

   //La nuova recensione punterà all'elemento che è in testa alla lista
   tmp->next = utente->valutazioni; 
	
   //Rendo la nuova recensione il primo elemento della lista (va in testa)
   utente->valutazioni = tmp;
}

//Inserisce le Prenotazioni in testa
void insertPre(struct utente *utente,char name[],int id_rest, int giorno, int mese, int anno, int posti_prenotati){
   //crea un link allo heap per la recensione
   
   struct prenotazione *tmp = (struct prenotazione*) malloc(sizeof(struct prenotazione));//(struct recensione*) puntatore della malloc della classe recensione
   strcpy(tmp->username,name);
   tmp->id_rest = id_rest;
   tmp->giorno = giorno;
   tmp->mese = mese;
   tmp->anno = anno;
   tmp->posti_prenotati = posti_prenotati;
   tmp->next = NULL;

   //La nuova prenotazione punterà all'elemento che è in testa alla lista
   tmp->next = utente->p_prenotabili; 
	
   //Rendo la nuova prenotazione il primo elemento della lista (va in testa)
   utente->p_prenotabili = tmp;
}

//ottengo i posti disponibili in base alle prenotazioni già fatte per quel giorno
int GetPPre(int id_rest, int giorno, int mese){
   struct tm tm;
   tm = getime();
   int posti_disponibili;
   ristorante *ristoInteressato = hash_Table_LookRistorante(id_rest);
   posti_disponibili = ristoInteressato->p_prenotabili;
   for(int i = 0; i<TABLE_SIZE; i++){
      if(hash_table_utenti[i]!=NULL){
         struct prenotazione *tmp = hash_table_utenti[i]->p_prenotabili;
   
         //Aggiorno i valori delle prenotazione del risto
         while(tmp != NULL) {
            ristorante *risto = hash_Table_LookRistorante(tmp->id_rest);

            if(risto->codice_id == id_rest && tmp->giorno == giorno && tmp->mese == mese)
               posti_disponibili =  posti_disponibili - tmp->posti_prenotati;
         
            tmp = tmp->next;
         }
      }
   }
   return posti_disponibili;
}

//aggiorna la media voti di un ristorante
void updateMedia(struct ristorante *ristoranti){

   float totale = 0; int j = 0;

   for(int i = 0; i<TABLE_SIZE; i++){
      //prendo le recensioni di ogni utente per ciclo
      if(hash_table_utenti[i] != NULL){
         struct recensione *tmp = hash_table_utenti[i]->valutazioni;

         
         while(tmp != NULL) {
            //sommo i valori delle recensioni se comparate si equivalgono
            if(tmp->id_rest == ristoranti->codice_id){
               totale += tmp->val;
               j++;
            }
            tmp = tmp->next;
         }
      }
   }
   if(totale>0)
   {
      float media = totale/j;
      //riporto il voto effettuale della media al ristorante interessato
      ristoranti->media_voti = media;
   }
}

//aggiorna la media voti di un utente
void updateUserMedia(struct utente *utenti){

   float totale = 0; int j = 0;
   struct recensione *tmp = utenti->valutazioni;

      //Aggiorno i valore delle prenotazione del risto
      while(tmp != NULL) {
         totale += tmp->val;
         j++;
         tmp = tmp->next;
      }
   float media = (float)totale/j;
   utenti->media_voti = media;
}


//Funzioni complementari o di debug

//Display the list
void StampaRec(struct utente *utente){
   
   struct recensione *tmp = utente->valutazioni;
  
   printf("------- RECENSIONI DI %s ---------\n", tmp->username);

   while(tmp != NULL) {
      printf("id ristorante : %d, ", tmp->id_rest);
      printf("Voto: %d\n", tmp->val);
      tmp = tmp->next;
   }
}

void StampaPre(struct utente *utente){
   
   struct prenotazione *tmp = utente->p_prenotabili;
   //start from the beginning

   printf("------- PRENOTAZIONI DI %s ---------\n", tmp->username);

   while(tmp != NULL) {
      printf("id ristorante : %d, ", tmp->id_rest);
      printf("giorno: %d, ", tmp->giorno);
      printf("mese: %d, ", tmp->mese);
      printf("anno: %d\n", tmp->anno);
      tmp = tmp->next;
   }
}

//controlla se il ristorante del codice id immesso esiste, se c'è già la stessa prenotazione in quel giorno
int ControlloPre(struct utente *utente, int id_rest, int giorno, int mese, int anno){
   
   struct prenotazione *tmp = utente->p_prenotabili;
   ristorante *ri = hash_Table_LookRistorante(id_rest);
   //start from the beginning
   if(ri!=NULL){ 
      while(tmp != NULL) {
         if(tmp->id_rest == id_rest && tmp->giorno == giorno && tmp->mese == mese && tmp->anno == anno)
         {
            MessageBox(NULL, "PRENOTAZIONE GIA' ESISTENTE", " ERROR!", MB_ICONERROR);
            return -1;
         } 
         tmp = tmp->next;
      }
   }
   else{
      MessageBox(NULL, "il ristorante inserito non ESISTE!", "ERROR!", MB_ICONERROR);
      return -1;
   }
   return 1;
}

//controllo di prenotazione e se esiste(allora lo ritorna) in altri casi restituisce errori
prenotazione *ControlloMPre(struct utente *utente, int id_rest, int giorno, int mese, int anno){
   
   struct prenotazione *tmp = utente->p_prenotabili;
   ristorante *ri = hash_Table_LookRistorante(id_rest);
   //start from the beginning
   if(ri!=NULL){ 
      while(tmp != NULL) {
         if(tmp->id_rest == id_rest && tmp->giorno == giorno && tmp->mese == mese && tmp->anno == anno)
         {
            return tmp;
         } 
         tmp = tmp->next;
      }
   }
   else{
      MessageBox(NULL, "il ristorante inserito non ESISTE!", "ERROR!", MB_ICONERROR);
      return NULL;
   }
}

//disdice la prenotazione
int CancellaCPre(struct utente *utente, int id_rest, int giorno, int mese){
   
   struct prenotazione *tmp = utente->p_prenotabili;
   ristorante *ri = hash_Table_LookRistorante(id_rest);
   //start from the beginning
   if(ri!=NULL){ 
      struct prenotazione *old = NULL;
      bool entered = false;
      while(tmp != NULL){
         if(tmp->id_rest == id_rest && tmp->giorno == giorno && tmp->mese == mese){
            entered = true;
            if(old == NULL)
               utente->p_prenotabili = tmp->next;
            else    
               old->next = tmp->next;
            free(tmp);
            tmp = NULL;
            break;
         }
         old = tmp;
         tmp = tmp->next;
      }
      if(entered == false){
         MessageBox(NULL, "PRENOTAZIONE NON ESISTENTE", "ERROR!", MB_ICONERROR);
         return -1;
      }
   }
   else{
      MessageBox(NULL, "il ristorante inserito non ESISTE!", "ERROR!", MB_ICONERROR);
      return -1;
   }
   
}

//controlla se il ristorante del codice id immesso esiste, se c'è già la stessa recensione
int ControlloRec(struct utente *utente, int id_rest, int voto){
   
   struct recensione *tmp = utente->valutazioni;
   ristorante *ri = hash_Table_LookRistorante(id_rest);
   //start from the beginning
   if(ri!=NULL){ 
      while(tmp != NULL) {
         if(tmp->id_rest == id_rest)
         {
            MessageBox(NULL, "RECENSIONE GIA' ESISTENTE", " ERROR!", MB_ICONERROR);
            return -1;
         } 
         tmp = tmp->next;
      }
   }
   else{
      MessageBox(NULL, "il ristorante inserito non ESISTE!", "ERROR!", MB_ICONERROR);
      return -1;
   }
   return 1;
}
//elimina le prenotazioni per utente
void DelPreUT(char *nome){
   
   utente *utente = hash_Table_LookUtente(nome);
   struct prenotazione *tmp = utente->p_prenotabili;
   
   //start from the beginning
   while(tmp != NULL) {
      prenotazione *nextemp; 
      nextemp = tmp->next;
      free(tmp);
      tmp = nextemp;
   }
}

//elimina le recensioni per utente
void DelRecUT(char *nome){
   
   utente *utente = hash_Table_LookUtente(nome);
   struct recensione *tmp = utente->valutazioni;
   
   //start from the beginning
   while(tmp != NULL) {
      recensione *nextemp; 
      nextemp = tmp->next;
      free(tmp);
      tmp = nextemp;
   }
}

//cancella le prenotazioni riferite a un determinato ristorante
void DelPreRI(int codice_id)
{
   for(int i = 0; i<TABLE_SIZE; i++){
      //prendo le recensioni di ogni utente per ciclo
      if(hash_table_utenti[i] != NULL){
         struct prenotazione *tmp = hash_table_utenti[i]->p_prenotabili;

         while(tmp != NULL){
            prenotazione *nextemp;
            nextemp = tmp->next;
            if(tmp->id_rest == codice_id){
               free(tmp);
            }
            tmp = nextemp;
         }
      }
   }
}

//cancella le prenotazioni riferite a un determinato ristorante
void DelRecRI(int codice_id)
{
   for(int i = 0; i<TABLE_SIZE; i++){
      //prendo le recensioni di ogni utente per ciclo
      if(hash_table_utenti[i] != NULL){
         struct recensione *tmp = hash_table_utenti[i]->valutazioni;

         while(tmp != NULL) {
            recensione *nextemp; 
            nextemp = tmp->next;
            if(tmp->id_rest == codice_id){
               free(tmp);
            }
            tmp = nextemp;
         }
      }
   }
}

//Stampa le recensioni
void printAllRec()
{
    for(int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_utenti[i]!=NULL)
            StampaRec(hash_table_utenti[i]);
    }
}

//Stampa le prenotazioni
void printAllPre()
{
    for(int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_utenti[i]!=NULL)
            StampaPre(hash_table_utenti[i]);
    }
}

//va ad aggiornare tutte le "medie dei voti" dei ristornati acquisita in base alle recensioni
void UpdateAllMedia(){
    for(int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_ristoranti[i] != NULL)
            updateMedia(hash_table_ristoranti[i]);
    }
}

//va ad aggiornare tutte le "medie dei voti" degli utenti acquisita in base alle recensioni da loro riposte
void UpdateAllUserMedia(){
   for(int i = 0; i<TABLE_SIZE; i++){
        if(hash_table_utenti[i] != NULL)
            updateUserMedia(hash_table_utenti[i]);
    }
}

#endif