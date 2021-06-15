#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <commctrl.h>
#include "mystruct.h"
#include "hashtable.h"
#include "linkedlist.h"

//a livello locale delle finestre al di fuori della main window si potranno riutilizzare in quanto hanno le loro proprie procedure
#define UTENTE_NUOVO 1
#define HELP 2
#define UTENTE_MODIFICA 3
#define RISTORANTE_NUOVO 5
#define RISTORANTE_MODIFICA 6
#define UTENTE_CANCELLA 12
#define RISTORANTE_CANCELLA 13
#define AGG_UTENTE 8
#define AGG_RISTORANTE 9
#define CERCA_UT_RI 10
#define DEL_UI_RI 11
#define PRE_NUOVO 14
#define REC_NUOVO 15
#define CANCEL_PRE 16
#define MOD_RIC 17
#define MOD_PRE 18
#define RIC_RIST 19


//valori che ci ritornano utili hash/char
#define MAX_NAME 20
#define TABLE_SIZE 1000


//predichiarazione delle funzioni

//procedure dove vai a gestire i msg che vengono mandati dalla finestra
LRESULT CALLBACK WindowProcedure(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK RistProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK UtentiProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK ModUtentiProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK UtentiDelProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK RistoModProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK RistorantiDelProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);
LRESULT CALLBACK NullProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp);

//funzione che importa/esporta dai file
void IMPORT();
void EXPORT();

//funzione che cerca l'utente per il main
int Cerca_utM(HWND Hricerca);

//funzione che ricava il tempo
struct tm getime();

//funzione per controllare gli input
bool checkdigitf(char ricerca[]);
bool checkdigit(char ricerca[]);
bool checkalfa(char ricerca[]);

//funzioni visualizzazioni delle nostre finestre del menu
void displayAggUT(HWND hWnd);
void displayAggRI(HWND hWnd);
void displayModUT(HWND hWnd);
void displayModRI(HWND hWnd);
void displayDelUT(HWND hWnd);
void displayDelRI(HWND hWnd);

//registri associati
void RegisterUtentiClass(HINSTANCE HInst);
void RegisterRistClass(HINSTANCE HInst);
void RegisterModClass(HINSTANCE HInst);
void RegisterRIModClass(HINSTANCE HInst);
void RegisterDelUtentiClass(HINSTANCE HInst);
void RegisterDelRistoClass(HINSTANCE HInst);
void RegisterNullClass(HINSTANCE HInst);

void AddMenus(HWND);
void AddControls(HWND);
void EnableAllWindow(HWND hWnd);
void DeleteAllWindow();

HWND HMainWindow;//handler per gestire quando disattivare/attivare l'utilizzo della finestra principale

//HASH TABLE
utente *hash_table_utenti[TABLE_SIZE];
ristorante *hash_table_ristoranti[TABLE_SIZE];

//struct time globale così da poterla utilizzare ovunque mi serva sapere l'anno, il giorno


int WINAPI WinMain(HINSTANCE Hinst, HINSTANCE hPrecInst, LPSTR args, int ncmdshow)
{
        WNDCLASSW wc = {0};
        //valori che identificano il tipo di finestra il cursore e così via
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.hCursor = LoadCursor(NULL,IDC_ARROW);

        //istanza generale delle finestre che utilizziamo anche per registrare le finestre al di
        //fuori del main(devono stare nella stessa istanza)
        wc.hInstance = Hinst;

        wc.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        wc.lpszClassName = L"myWindowClass";

        //La funzione che gestirà i messaggi della finestra, (ES."quando premi il bottone il menu fai x")
        wc.lpfnWndProc = WindowProcedure;
        //ShowWindow(GetConsoleWindow(), SW_HIDE);
	if(!RegisterClassW(&wc))
             return -1;
        //andiamo ad aggiungere alla nostra istanza i registri delle finestre secondarie figlie
        RegisterUtentiClass(Hinst);
        RegisterRistClass(Hinst);
        RegisterModClass(Hinst);
        RegisterDelUtentiClass(Hinst);
        RegisterRIModClass(Hinst);
        RegisterDelRistoClass(Hinst);
        RegisterNullClass(Hinst);

        //dichiarazione della finestra principale la associamo ad un handler perchè ci servirà successivamente
        HMainWindow = CreateWindowW(L"myWindowClass", L"RistBase", WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME | WS_VISIBLE , 500, 100, 1000, 900, NULL, NULL, NULL, NULL);

        //inizializziamo la classe di msg che poi verrà gestito riempito utilizzato dalle finestre
        MSG msg = {0};
        while(GetMessage(&msg,NULL,(UINT_PTR)NULL,(UINT_PTR)NULL)){
            //gestione dei messaggi che manda la finestra, ottenendolo, traducendolo e inviandolo.
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
        }
        return 0;
}

//handlers tra la procedure e la addcontrol
HWND HricercaMain, HWinRicerca, HPreid, HPreGio, HPreMese, HPreAnno, hPreMain, hbutton, hTBid, hTBpre,hRecIDb,hRecID,hRecVotob,hRecVoto,
hRecBut,hCPreIDb,hCPreID,hCPreGb,hCPreG,hCPreMb,hCPreM,hCPreBut,hMPreIDb, hMPreID, hMPreGb, hMPreG, hMPreMb, hMPreM, hMPreBut, hMPreAb, hMPreA,
hPPreb, hPPre, hMPPre, hMPPreb, hMPreBut2, hRRic, hRecb, hRecTextb, hComboBox;

char ricerca[20];

int oldidrest, oldmese, oldgiorno, oldanno;
LRESULT CALLBACK WindowProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
		case WM_COMMAND:
			switch(wp){
				case UTENTE_NUOVO:
                    MessageBeep(MB_RIGHT);
                    displayAggUT(hWnd);
					break;
                case RISTORANTE_NUOVO:
                    MessageBeep(MB_RIGHT);
                    displayAggRI(hWnd);
                    break;
				case UTENTE_MODIFICA:
                   MessageBeep(MB_RIGHT);
                   displayModUT(hWnd);
                   break;
                case RISTORANTE_MODIFICA:
                   MessageBeep(MB_RIGHT);
                   displayModRI(hWnd);
                   break;
                case UTENTE_CANCELLA:
                    MessageBeep(MB_RIGHT);
                    displayDelUT(hWnd);
                    break;
                case RISTORANTE_CANCELLA:
                    MessageBeep(MB_RIGHT);
                    displayDelRI(hWnd);
                    break;
                case CERCA_UT_RI:
                {
                    MessageBeep(MB_RIGHT);
                    //mostra varie message box in base all output, se ritorna 1 vuol dire che ha trovato l'utente ricercato
                    if(Cerca_utM(HricercaMain)==1){
                        //mettiamo la ricerca nella variabile globale così da passarla alle altre funzioni
                        GetWindowText(HricercaMain, ricerca, 20);
                        //Riattiva tutte le finestre
                        EnableAllWindow(hWnd);
                        //cancella i dati delle box ogni volta che ricerca
                        DeleteAllWindow();
                    }
                }
                break;
                case PRE_NUOVO:
                {
                    struct tm tm  = getime();
                    utente *ut = hash_Table_LookUtente(ricerca);
                    char tmp[20];
                    int id_rest, giorno, mese, anno, posti_pren, posti_disp;
                    GetWindowText(HPreid, tmp, 10);
                    id_rest = atoi(tmp);
                    GetWindowText(HPreGio, tmp, 10);
                    giorno = atoi(tmp);
                    GetWindowText(HPreMese, tmp, 10);
                    mese = atoi(tmp);
                    GetWindowText(HPreAnno, tmp, 10);
                    anno = atoi(tmp);
                    GetWindowText(hPPre, tmp, 10);
                    posti_pren = atoi(tmp);
                    posti_disp = GetPPre(id_rest, giorno, mese);
                    if(giorno<=tm.tm_mday && mese==tm.tm_mon){
                        char str[50], str2[20];
                        strcpy(str, "INSERISCI UNA DATA MAGGIORE DEL GIORNO: ");
                        itoa(tm.tm_mday, str2, 10);
                        strcat(str, str2);
                        MessageBox(NULL,str,"ERROR",MB_ICONERROR);
                    }
                    else
                    {
                        if(giorno<32 && giorno>0 && mese>tm.tm_mon-1 && mese<13 && anno<tm.tm_year+1 && anno>tm.tm_year-1 && posti_disp>=posti_pren && posti_pren !=0)
                        {
                            if(ControlloPre(ut,id_rest,giorno,mese,anno)==1){
                                insertPre(ut,ricerca,id_rest,giorno,mese,anno,posti_pren);
                                MessageBox(NULL, "PRENOTAZIONE INSERITA","SUCCESS", MB_ICONINFORMATION);
                                SetWindowText(HPreid,"");
                                SetWindowText(HPreGio,"");
                                SetWindowText(HPreMese,"");
                                SetWindowText(HPreAnno,"");
                                SetWindowText(hPPre,"");
                            }
                        }
                        else
                        {
                            if(posti_disp<posti_pren){
                                char str[50], str2[20];
                                strcpy(str, "I POSTI PRENOTABILI SONO FINITI/MINORI DELLA PRENOTAZIONE (disponibili : ");
                                itoa(posti_disp,str2, 10);
                                strcat(str,str2);
                                strcat(str,")");
                                MessageBox(NULL, str, "ERROR!", MB_ICONERROR);
                            }
                            else
                            MessageBox(NULL, "INSERISCI UNA DATA CORRETTA", "ERROR!", MB_ICONERROR);
                        }
                    }
                    //controlliamo se la data inserita è giusta (possibile prenotare anche il prossimo anno per questo +2)
                    
                }
                break;
                case REC_NUOVO:
                {
                    int id_rest, voto;
                    char tmp[20];
                    utente *ut = hash_Table_LookUtente(ricerca);
                    GetWindowText(hRecID, tmp, 10);
                    id_rest = atoi(tmp);
                    GetWindowText(hRecVoto, tmp, 10);
                    voto = atoi(tmp);
                    if(voto<11){
                        if(ControlloRec(ut, id_rest, voto)==1)
                        {
                            insertRec(ut,ricerca, id_rest, voto);
                            UpdateAllMedia();
                            UpdateAllUserMedia();
                            MessageBox(NULL, "RECENSIONE INSERITA","SUCCESS", MB_ICONINFORMATION);
                            SetWindowText(hRecVoto, "");
                            SetWindowText(hRecID, "");
                        }
                    }
                    else{
                        MessageBox(NULL, "VOTO NON VALIDO","ERROR!", MB_ICONERROR);
                    }
                }
                break;
                case CANCEL_PRE:
                {
                    struct tm tm  = getime();
                    utente *ut = hash_Table_LookUtente(ricerca);
                    char tmp[20];
                    int id_rest, giorno, mese;
                    GetWindowText(hCPreID, tmp, 10);
                    id_rest = atoi(tmp);
                    GetWindowText(hCPreG, tmp, 10);
                    giorno = atoi(tmp);
                    GetWindowText(hCPreM, tmp, 10);
                    mese = atoi(tmp);
                    if(giorno<=tm.tm_mday && mese==tm.tm_mon){
                        char str[50], str2[20];
                        strcpy(str, "INSERISCI UNA DATA MAGGIORE DEL GIORNO: ");
                        itoa(tm.tm_mday, str2, 10);
                        strcat(str, str2);
                        MessageBox(NULL,str,"ERROR",MB_ICONERROR);
                    }
                    else
                    {
                        if(giorno<32 && giorno>0 && mese>tm.tm_mon-1 && mese<13){
                            if(CancellaCPre(ut,id_rest,giorno,mese)!=-1){
                                MessageBox(NULL, "PRENOTAZIONE ELIMINATA","SUCCESS", MB_ICONINFORMATION);
                                SetWindowText(hCPreG, "");
                                SetWindowText(hCPreID, "");
                                SetWindowText(hCPreM, "");
                            }
                        }
                        else if(giorno==tm.tm_mday)
                            MessageBox(NULL,"PRENOTAZIONE DISDIBILE SE FATTO ALMENO UN GIORNO PRIMA!", "ERROR!", MB_ICONERROR);
                        else
                            MessageBox(NULL,"DATI INSERITI NON VALIDI", "ERROR!", MB_ICONERROR);
                    }
                }
                break;
                case MOD_RIC:
                {  
                    struct tm tm  = getime();
                    utente *ut = hash_Table_LookUtente(ricerca);
                    prenotazione *tmp;
                    char string[20];
                    GetWindowText(hMPreID, string, 10);
                    oldidrest = atoi(string);
                    GetWindowText(hMPreG, string, 10);
                    oldgiorno = atoi(string);
                    GetWindowText(hMPreM, string, 10);
                    oldmese = atoi(string);
                    GetWindowText(hMPreA, string, 10);
                    oldanno = atoi(string);
                    if((tmp = ControlloMPre(ut,oldidrest,oldgiorno,oldmese,oldanno))!=NULL){
                        MessageBox(NULL,"PRENOTAZIONE TROVATA!", "SUCCESS", MB_ICONINFORMATION);
                        EnableWindow(hMPreBut, false);
                        EnableWindow(hMPreID, false);
                        EnableWindow(hMPreBut2, true);
                    }   
                    else
                        MessageBox(NULL,"PRENOTAZIONE NON TROVATA!", "ERROR!", MB_ICONERROR);
                }
                break;
                case MOD_PRE:
                {
                    struct tm tm  = getime();
                    utente *ut = hash_Table_LookUtente(ricerca);
                    prenotazione *tmp = ControlloMPre(ut,oldidrest,oldgiorno,oldmese,oldanno);
                    char string[20];
                    int giorno, mese, anno, posti_pren, posti_disp;
                    GetWindowText(hMPreG, string, 10);
                    giorno = atoi(string);
                    GetWindowText(hMPreM, string, 10);
                    mese = atoi(string);
                    GetWindowText(hMPreA, string, 10);
                    anno = atoi(string);
                    GetWindowText(hMPPre, string, 10);
                    posti_pren = atoi(string);
                    posti_disp = GetPPre(oldidrest,giorno,mese);
                    if(giorno<=tm.tm_mday && mese==tm.tm_mon){
                        char str[50], str2[20];
                        strcpy(str, "INSERISCI UNA DATA MAGGIORE DEL GIORNO: ");
                        itoa(tm.tm_mday, str2, 10);
                        strcat(str, str2);
                        MessageBox(NULL,str,"ERROR",MB_ICONERROR);
                    }
                    else
                    {
                        if(giorno<32 && giorno>0 && mese>tm.tm_mon-1 && mese<13 && anno<tm.tm_year+1 && anno>tm.tm_year-1 && posti_disp>=posti_pren && posti_pren !=0)
                        {
                            if(ControlloPre(ut,oldidrest,giorno,mese,anno)==1){
                                
                                tmp->giorno = giorno;
                                tmp->mese = mese;
                                tmp->anno = anno;
                                tmp->posti_prenotati = posti_pren;
                                MessageBox(NULL, "PRENOTAZIONE INSERITA","SUCCESS", MB_ICONINFORMATION);
                                SetWindowText(hMPreID,"");
                                SetWindowText(hMPreG,"");
                                SetWindowText(hMPreM,"");
                                SetWindowText(hMPreA,"");
                                SetWindowText(hMPPre,"");
                                EnableWindow(hMPreID, true);
                                EnableWindow(hMPreBut, true);
                                EnableWindow(hMPreBut2, false);
                            }
                        }
                        else
                        {
                            if(posti_disp<posti_pren){
                                char str[70], str2[20];
                                strcpy(str, "I POSTI PRENOTABILI SONO FINITI/MINORI DELLA PRENOTAZIONE (disponibili : ");
                                itoa(posti_disp,str2, 10);
                                strcat(str,str2);
                                strcat(str,")");
                                MessageBox(NULL, str, "ERROR!", MB_ICONERROR);
                            }
                            else
                            MessageBox(NULL, "INSERISCI UNA DATA CORRETTA", "ERROR!", MB_ICONERROR);
                        }
                    }
                }
                break;
                case RIC_RIST:
                {
                    char ricercaR[20], scelta[20];
                    GetWindowText(hRRic, ricercaR, 20);
                    GetWindowText(hComboBox, scelta, 20);
                    if(strcmp(scelta, "Media")==0){
                        bool check = false;
                        float media;
                        check = checkdigitf(ricercaR);
                        media = atof(ricercaR);
                        if(check == true && media<11 && media>0){
                            McercaRI(media);
                        }
                        else
                        {
                            MessageBox(NULL, "VALORI IMMESSI NON VALIDI", "ERROR!", MB_ICONERROR);
                        }
                    }
                    else if(strcmp(scelta, "Localita'")==0)
                    {
                        bool check = false;
                        check = checkalfa(ricercaR);
                        if(check == true){
                            LcercaRI(ricercaR);
                        }
                        else
                        {
                            MessageBox(NULL, "VALORI IMMESSI NON VALIDI", "ERROR!", MB_ICONERROR);
                        }
                    }
                    else if(strcmp(scelta, "fascia di prezzo")==0)
                    {
                        bool check = false;
                        check = checkdigit(ricercaR);
                        if(check == true){
                            int fascia = atoi(ricercaR);
                            fcercaRI(fascia);
                        }
                        else
                        {
                            MessageBox(NULL, "VALORI IMMESSI NON VALIDI", "ERROR!", MB_ICONERROR);
                        }
                    }
                }
                break;
				case HELP:
						MessageBox(NULL,"programma che ti permette di prenotare online, andra' automaticamente a caricare e esportare i dati sui ristoranti e utenti. ha varie funzioni tra le quali: la possibilita' dai sottomenu di cancellare/aggiungere/modificare i dati degli utenti e ristoranti, mentre nella finestra principale quello di disdire la prenotazione, prenotare, esprimere una recensione e di cercare un ristorante." ," Come Funziona?", MB_ICONQUESTION);
						break;
            }
		break;
	case WM_CREATE:
    {
        IMPORT();
		AddMenus(hWnd);
		AddControls(hWnd);
    }
	break;
    case WM_DESTROY:
    //__________DEBUG_________
        print_hashRisto();
        print_hashUtenti();
        printAllPre();
        printAllRec();
    //__________DEBUG_________
        EXPORT();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd,msg,wp,lp);
    }
}

void AddMenus(HWND hWnd){
    HMENU hMenu = CreateMenu(), hUtenteMenu = CreateMenu(), hRistoranteMenu = CreateMenu();

	AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hUtenteMenu,"Utente");
    AppendMenu(hUtenteMenu, MF_STRING, UTENTE_NUOVO, "Nuovo");
    AppendMenu(hUtenteMenu, MF_STRING, UTENTE_MODIFICA, "Modifica");
    AppendMenu(hUtenteMenu, MF_STRING, UTENTE_CANCELLA, "Cancella");

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hRistoranteMenu,"Ristoranti");
    AppendMenu(hRistoranteMenu, MF_STRING, RISTORANTE_NUOVO, "Nuovo");
    AppendMenu(hRistoranteMenu, MF_STRING, RISTORANTE_MODIFICA, "Modifica");
    AppendMenu(hRistoranteMenu, MF_STRING, RISTORANTE_CANCELLA, "Cancella");

    AppendMenu(hMenu,MF_STRING,HELP,"help");
	SetMenu(hWnd, hMenu);//aggiungiamo il "hMenu" menu all "hWnd" handler (che gestisce la finestra).
}

void AddControls(HWND hWnd)//dichiariamo la nostra addcontrol e processeremo ovviamente wm_create ove si crea la finestra
{
    //-------------------------------------UTENTE----------------------------------------------------------------------------
    HWinRicerca = CreateWindowW(L"static",L"Ricerca Utente",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,350,40,250,135,hWnd,NULL,NULL,NULL);
	CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,30,35,50,20,HWinRicerca,NULL,NULL,NULL);
	HricercaMain = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL,80,35,130,20,HWinRicerca,NULL,NULL,NULL);
    CreateWindowW(L"Button",L"Conferma",WS_VISIBLE | WS_CHILD ,435,115,70,40,hWnd,(HMENU)CERCA_UT_RI,NULL,NULL);
    //------------------------------------------------------------------------------------------------------------------------

    //--------------------------------Prenotazione--------------------------------------------------
    hPreMain = CreateWindowW(L"myNullClass",L"Nuova Prenotazione", WS_DISABLED |WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,50,225,250,220,hWnd,NULL,NULL,NULL);
    CreateWindowW(L"static",L"Nuova Prenotazione",WS_VISIBLE | WS_CHILD ,110,240,170,20,hWnd,NULL,NULL,NULL);
    hTBid = CreateWindowW(L"static",L"id Rist :",WS_VISIBLE |WS_DISABLED | WS_CHILD | WS_BORDER | SS_CENTER,110,265,60,20,hWnd,NULL,NULL,NULL);
	HPreid = CreateWindowW(L"edit", NULL, WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL|ES_NUMBER,170,265,60,20,hWnd,NULL,NULL,NULL);
    hTBpre = CreateWindowW(L"static",L"Data Prenotazione :",WS_DISABLED |WS_VISIBLE | WS_CHILD,110,295,130,20,hWnd,NULL,NULL,NULL);
	HPreGio = CreateWindowW(L"edit", L"d",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,110,325,30,20,hWnd,NULL,NULL,NULL);
    HPreMese = CreateWindowW(L"edit", L"m",WS_DISABLED | WS_BORDER |WS_VISIBLE | WS_CHILD  | ES_AUTOVSCROLL|ES_NUMBER,155,325,30,20,hWnd,NULL,NULL,NULL);
	HPreAnno = CreateWindowW(L"edit", L"y",WS_DISABLED | WS_VISIBLE | WS_CHILD  | ES_AUTOVSCROLL|ES_NUMBER| WS_BORDER,200,325,40,20,hWnd,NULL,NULL,NULL);
    hPPreb = CreateWindowW(L"static",L"Posti Prenotati :",WS_DISABLED |WS_VISIBLE | WS_CHILD,110,360,110,20,hWnd,NULL,NULL,NULL);
	hPPre = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,220,360,30,20,hWnd,NULL,NULL,NULL);
    hbutton = CreateWindowW(L"Button",L"Conferma",WS_DISABLED |WS_VISIBLE | WS_CHILD,140,385,70,40,hWnd,(HMENU)PRE_NUOVO,NULL,NULL);
    

    //limitiamo il massimo degli input inseribili nelle edit box di giorno/mese
    const int maximum_length = 2;
    SendMessage(HPreGio, EM_SETLIMITTEXT, maximum_length, 0);
    SendMessage(HPreMese, EM_SETLIMITTEXT, maximum_length, 0);
    SendMessage(hPPre, EM_SETLIMITTEXT, maximum_length, 0);
    //---------------------------------------------------------------------------------------------

    //--------------------------------Recensione--------------------------------------------------
    CreateWindowW(L"myNullClass",L"Nuova Recensione", WS_DISABLED |WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,350,225,250,220,hWnd,NULL,NULL,NULL);
    CreateWindowW(L"static",L"Nuova Recensione",WS_VISIBLE | WS_CHILD ,415,240,170,20,hWnd,NULL,NULL,NULL);
    hRecIDb = CreateWindowW(L"static",L"id Rist :",WS_VISIBLE |WS_DISABLED | WS_CHILD | WS_BORDER | SS_CENTER,415,275,60,20,hWnd,NULL,NULL,NULL);
	hRecID = CreateWindowW(L"edit", NULL, WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL|ES_NUMBER,475,275,60,20,hWnd,NULL,NULL,NULL);
    hRecVotob = CreateWindowW(L"static",L"Voto :",WS_DISABLED |WS_VISIBLE | WS_CHILD,415,315,40,20,hWnd,NULL,NULL,NULL);
	hRecVoto = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,455,315,30,20,hWnd,NULL,NULL,NULL);
    hRecBut = CreateWindowW(L"Button",L"Conferma",WS_DISABLED |WS_VISIBLE | WS_CHILD,430,360,70,40,hWnd,(HMENU)REC_NUOVO,NULL,NULL);
    
    //limitiamo il massimo degli input inseribili nelle edit box di giorno/mese
    SendMessage(hRecVoto, EM_SETLIMITTEXT, maximum_length, 0);
    //---------------------------------------------------------------------------------------------

    //--------------------------------DISDIRE PRENOTAZIONE--------------------------------------------------
    CreateWindowW(L"myNullClass",L"Annulla Prenotazione", WS_DISABLED |WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,650,225,250,220,hWnd,NULL,NULL,NULL);
    CreateWindowW(L"static",L"Annulla Prenotazione",WS_VISIBLE | WS_CHILD ,715,240,170,20,hWnd,NULL,NULL,NULL);
    hCPreIDb = CreateWindowW(L"static",L"id Rist :",WS_VISIBLE |WS_DISABLED | WS_CHILD | WS_BORDER | SS_CENTER,715,275,60,20,hWnd,NULL,NULL,NULL);
	hCPreID = CreateWindowW(L"edit", NULL, WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL|ES_NUMBER,775,275,60,20,hWnd,NULL,NULL,NULL);
    hCPreGb = CreateWindowW(L"static",L"Giorno :",WS_DISABLED |WS_VISIBLE | WS_CHILD,715,315,60,20,hWnd,NULL,NULL,NULL);
	hCPreG = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,775,315,30,20,hWnd,NULL,NULL,NULL);
    hCPreMb = CreateWindowW(L"static",L"Mese :",WS_DISABLED |WS_VISIBLE | WS_CHILD,720,345,50,20,hWnd,NULL,NULL,NULL);
	hCPreM = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,775,345,30,20,hWnd,NULL,NULL,NULL);
    hCPreBut = CreateWindowW(L"Button",L"Conferma",WS_DISABLED |WS_VISIBLE | WS_CHILD,730,380,70,40,hWnd,(HMENU)CANCEL_PRE,NULL,NULL);
    
    //limitiamo il massimo degli input inseribili nelle edit box di giorno/mese
    SendMessage(hCPreM, EM_SETLIMITTEXT, maximum_length, 0);
    SendMessage(hCPreG, EM_SETLIMITTEXT, maximum_length, 0);
    //---------------------------------------------------------------------------------------------

    //--------------------------------MODIFICA PRENOTAZIONE--------------------------------------------------
    CreateWindowW(L"myNullClass",L"Modifica Prenotazione", WS_DISABLED |WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,150,465,315,270,hWnd,NULL,NULL,NULL);
    CreateWindowW(L"static",L"Modifica Prenotazione",WS_VISIBLE | WS_CHILD ,250,480,170,20,hWnd,NULL,NULL,NULL);
    hMPreIDb = CreateWindowW(L"static",L"id Rist :",WS_VISIBLE |WS_DISABLED | WS_CHILD | WS_BORDER | SS_CENTER,250,515,60,20,hWnd,NULL,NULL,NULL);
	hMPreID = CreateWindowW(L"edit", NULL, WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL|ES_NUMBER,310,515,60,20,hWnd,NULL,NULL,NULL);
    hMPreGb = CreateWindowW(L"static",L"Giorno :",WS_DISABLED |WS_VISIBLE | WS_CHILD,200,555,60,20,hWnd,NULL,NULL,NULL);
	hMPreG = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,260,555,30,20,hWnd,NULL,NULL,NULL);
    hMPreMb = CreateWindowW(L"static",L"Mese :",WS_DISABLED |WS_VISIBLE | WS_CHILD,330,555,50,20,hWnd,NULL,NULL,NULL);
	hMPreM = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,380,555,30,20,hWnd,NULL,NULL,NULL);
    hMPreAb = CreateWindowW(L"static",L"Anno :",WS_DISABLED |WS_VISIBLE | WS_CHILD,200,590,50,20,hWnd,NULL,NULL,NULL);
	hMPreA = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,260,590,40,20,hWnd,NULL,NULL,NULL);
    hMPPreb = CreateWindowW(L"static",L"*Posti :",WS_DISABLED |WS_VISIBLE | WS_CHILD,330,590,50,20,hWnd,NULL,NULL,NULL);
	hMPPre = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | ES_NUMBER| WS_BORDER,380,590,30,20,hWnd,NULL,NULL,NULL);
    hMPreBut = CreateWindowW(L"Button",L"Ricerca",WS_DISABLED| WS_VISIBLE | WS_CHILD,230,630,70,40,hWnd,(HMENU)MOD_RIC,NULL,NULL);
    hMPreBut2 = CreateWindowW(L"Button",L"Conferma",WS_DISABLED |WS_VISIBLE | WS_CHILD,330,630,70,40,hWnd,(HMENU)MOD_PRE,NULL,NULL);
    CreateWindowW(L"static",L"*: Posti non è richiesto per la ricerca",WS_VISIBLE | WS_CHILD,180,690,250,20,hWnd,NULL,NULL,NULL);
    
    //limitiamo il massimo degli input inseribili nelle edit box di giorno/mese
    SendMessage(hCPreM, EM_SETLIMITTEXT, maximum_length, 0);
    SendMessage(hCPreG, EM_SETLIMITTEXT, maximum_length, 0);
    SendMessage(hMPPre, EM_SETLIMITTEXT, maximum_length, 0);
    //---------------------------------------------------------------------------------------------

    //--------------------------------RICERCA RISTORANTE--------------------------------------------------
    CreateWindowW(L"myNullClass",L"Ricerca Ristorante", WS_DISABLED |WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER|WS_THICKFRAME,500,465,315,270,hWnd,NULL,NULL,NULL);
    CreateWindowW(L"static",L"Ricerca Ristorante",WS_VISIBLE | WS_CHILD ,590,480,170,20,hWnd,NULL,NULL,NULL);
	hRRic = CreateWindowW(L"edit", NULL, WS_DISABLED | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOVSCROLL,590,505,120,20,hWnd,NULL,NULL,NULL);
	
    hRecb = CreateWindowW(L"Button",L"Ricerca",WS_DISABLED| WS_VISIBLE | WS_CHILD,620,590,70,40,hWnd,(HMENU)RIC_RIST,NULL,NULL);
    hRecTextb = CreateWindowW(L"edit", L"",WS_DISABLED |WS_CHILD | WS_VISIBLE | WS_BORDER |ES_AUTOVSCROLL|ES_MULTILINE,575,640,170,80,hWnd,NULL,NULL,NULL);
    
    hComboBox = CreateWindowW(L"combobox", NULL,
        WS_DISABLED | WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        593, 545, 120, 200, hWnd, NULL, NULL, NULL);
    
    const TCHAR  *items[] = {TEXT("Media"), TEXT("Localita'"), TEXT("fascia di prezzo")};
    for(int i = 0; i<3; i++){
        SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)items[i]);
    }
    //---------------------------------------------------------------------------------------------

}

//Modal dialog utenti ↓

//ModalDialog Per l'aggiunta di utenti
//--------------------------------------------------------------------------

HWND Hnome, Hemail, Hnanno, Htradizione, Hfasciaprezzo; //handlers che puntano alle box, utili a ottenere il valore immesso
//nelle box
LRESULT CALLBACK UtentiProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case AGG_UTENTE:
                {
                    char nome[20], email[30], tradizione[20], temp[10];
                    //temp ci serve per convertire la stringa in int
                    int nanno = 0, fasciaprezzo = 0, mediavoti = 0;
                    GetWindowText(Hnome,nome,20);
				    GetWindowText(Hemail,email,30);
                    GetWindowText(Hnanno,temp,5);
                    nanno = atoi(temp);
				    GetWindowText(Htradizione,tradizione,20);
                    GetWindowText(Hfasciaprezzo,temp,5);
                    fasciaprezzo = atoi(temp);
                    if(strcmp(nome, "")==0 || strcmp(email, "")==0 || strcmp(tradizione, "")==0 || nanno == 0 || fasciaprezzo == 0){
                        MessageBox(NULL, "Inserisci i Valori Richiesti", " ERROR!", MB_ICONERROR);
                    }
                    else{
                        //------------------DEBUG------------
                        printf("nome : %s\nemail : %s\ntradizione : %s\nanno : %d\nfascia : %d\nmedia : %d\n", nome,email,tradizione,nanno,fasciaprezzo,mediavoti);
                        //-----------------------------------
                        utente ut;
                        strcpy(ut.username, nome); strcpy(ut.email, email); strcpy(ut.tradizione, tradizione); ut.n_anno = nanno;
                        ut.fasciadiprezzo = fasciaprezzo; ut.media_voti = 0;
                        if(hash_table_insertU(&ut)==1){
                            MessageBox(NULL, "INVALID DATA: L'UTENTE E' GIA' INSERITO", " ERROR!", MB_ICONSTOP);
                        }
                        //-----------------DEBUG-------------------
                        //print_hashUtenti();
                        //-----------------------------------------
                        MessageBox(NULL, "L'UTENTE E' STATO INSERITO", " SUCCESS!", MB_ICONEXCLAMATION);
                    }
                    break;
                }
            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterUtentiClass(HINSTANCE HInst){
    WNDCLASSW Utenti = {0};
        Utenti.hbrBackground = (HBRUSH)COLOR_WINDOW;
        Utenti.hCursor = LoadCursor(NULL,IDC_ARROW);
        Utenti.hInstance = HInst;
        Utenti.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        Utenti.lpszClassName = L"myUtentiClass";
        Utenti.lpfnWndProc = UtentiProcedure;
	    RegisterClassW(&Utenti);
}

void displayAggUT(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myUtentiClass",(LPCWSTR)"Nuovo Utente",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,425,330, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,55,50,50,20,HDLG,NULL,NULL,NULL);
    Hnome = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,105,50,250,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"email :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,55,80,50,20,HDLG,NULL,NULL,NULL);
    Hemail = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER ,105,80,250,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"tradizione :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,115,78,20,HDLG,NULL,NULL,NULL);
    Htradizione = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,130,115,225,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"fascia di prezzo :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,150,134,20,HDLG,NULL,NULL,NULL);
    Hfasciaprezzo = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,150,100,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"anno nascita :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,185,95,20,HDLG,NULL,NULL,NULL);
    Hnanno = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,150,185,120,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Aggiungi",WS_VISIBLE | WS_CHILD,140,220,100,40,HDLG,(HMENU)AGG_UTENTE,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------

//ModalDialog Per la modifica di utenti
//--------------------------------------------------------------------------

//handlers di ricerca (mod utenti/ristoranti) e di cancellazione (Del utenti/ristoranti)
HWND Hricerca, Hcancella;

LRESULT CALLBACK ModUtentiProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case CERCA_UT_RI:
                {
                    //--------------------DEBUG----------------
                    //print_hashUtenti();
                    //-----------------------------------------
                    char ricerca[20],tmp[10];
                    GetWindowText(Hricerca, ricerca, 20);
                    if(strcmp(ricerca, "") == 0)
                        MessageBox(NULL, "INSERISCI LA RICERCA", "ERROR!", MB_ICONERROR);
                    else{
                        //stringa minuscola in quanto nella hash le abbiamo minuscole
                        toLower(ricerca);
                        utente *ut;
                        ut = hash_Table_LookUtente(ricerca);
                        if(ut == NULL){
                            MessageBox(NULL, "UTENTE NON TROVATO", "ERROR!", MB_ICONERROR);
                        }
                        else
                        {
                            SetWindowText(Hemail, ut->email);
                            SetWindowText(Htradizione, ut->tradizione);
                            itoa(ut->fasciadiprezzo, tmp, 10);
                            SetWindowText(Hfasciaprezzo, tmp);
                            itoa(ut->n_anno, tmp, 10);
                            SetWindowText(Hnanno, tmp);
                            EnableWindow(Hricerca, false);
                        }
                    }
                }
                break;
                case AGG_UTENTE://riutilizzo lo stesso id siccome risparmio memoria non conviene occupare un altro id DEFINE
                {
                    char nome[20], email[30], tradizione[20], temp[10];
                    //temp ci serve per convertire la stringa in int
                    int nanno = 0, fasciaprezzo = 0, mediavoti = 0;
                    GetWindowText(Hricerca,nome,20);
				    GetWindowText(Hemail,email,30);
                    GetWindowText(Hnanno,temp,5);
                    nanno = atoi(temp);
				    GetWindowText(Htradizione,tradizione,20);
                    GetWindowText(Hfasciaprezzo,temp,5);
                    fasciaprezzo = atoi(temp);
                    if(strcmp(nome, "")==0 || strcmp(email, "")==0 || strcmp(tradizione, "")==0 || nanno == 0 || fasciaprezzo == 0){
                        MessageBox(NULL, "Inserisci i Valori Richiesti", " ERROR!", MB_ICONERROR);
                    }
                    else{
                        //------------------DEBUG------------
                        //printf("nome : %s\nemail : %s\ntradizione : %s\nanno : %d\nfascia : %d\nmedia : %d\n", nome,email,tradizione,nanno,fasciaprezzo,mediavoti);
                        //-----------------------------------
                        utente ut;
                        strcpy(ut.username, nome); strcpy(ut.email, email); strcpy(ut.tradizione, tradizione); ut.n_anno = nanno;
                        ut.fasciadiprezzo = fasciaprezzo; ut.media_voti = 0;
                        print_hashUtenti();
                        hash_table_ModU(&ut);
                        MessageBox(NULL, "UTENTE MODIFICATO! Puoi effettuare un altra ricerca", "SUCCESS", MB_ICONEXCLAMATION);
                        //-----------------DEBUG-------------------
                        //print_hashUtenti();
                        //-----------------------------------------
                        //setto le box a null per averle vuote e riabilito la barra di ricerca
                        SetWindowText(Hemail, NULL);
                        SetWindowText(Htradizione, NULL);
                        SetWindowText(Hfasciaprezzo, NULL);
                        SetWindowText(Hnanno, NULL);
                        EnableWindow(Hricerca, true);
                    }
                }
            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterModClass(HINSTANCE HInst){
    WNDCLASSW UtentiMod = {0};
        UtentiMod.hbrBackground = (HBRUSH)COLOR_WINDOW;
        UtentiMod.hCursor = LoadCursor(NULL,IDC_ARROW);
        UtentiMod.hInstance = HInst;
        UtentiMod.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        UtentiMod.lpszClassName = L"myModUtentiClass";
        UtentiMod.lpfnWndProc = ModUtentiProcedure;
	    RegisterClassW(&UtentiMod);
}


void displayModUT(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myModUtentiClass",(LPCWSTR)"Modifica Utente",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,400,390, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,75,30,50,20,HDLG,NULL,NULL,NULL);
    Hricerca = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,125,30,150,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Cerca",WS_VISIBLE | WS_CHILD,140,60,80,40,HDLG,(HMENU)CERCA_UT_RI,NULL,NULL);

    CreateWindowW(L"static",L"email :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,55,130,50,20,HDLG,NULL,NULL,NULL);
    Hemail = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER ,105,130,250,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"tradizione :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,165,78,20,HDLG,NULL,NULL,NULL);
    Htradizione = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,130,165,225,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"fascia di prezzo :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,200,134,20,HDLG,NULL,NULL,NULL);
    Hfasciaprezzo = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,200,100,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"anno nascita :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,235,95,20,HDLG,NULL,NULL,NULL);
    Hnanno = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,150,235,120,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Conferma",WS_VISIBLE | WS_CHILD,140,270,80,40,HDLG,(HMENU)AGG_UTENTE,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------

//ModalDialog Per la cancellazione di utenti
//--------------------------------------------------------------------------
LRESULT CALLBACK UtentiDelProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case DEL_UI_RI:
                {
                    char nome[20];
                    GetWindowText(Hcancella, nome, 20);
                    if(strcmp(nome, "")==0)
                        MessageBox(NULL, "INSERISCI I VALORI", "ERROR!", MB_ICONERROR);
                    else
                    {
                        if(hash_Table_LookUtente(nome)==NULL)
                        MessageBox(NULL,"UTENTE NON TROVATO", "ERROR!", MB_ICONERROR);
                        else
                        {
                            DelPreUT(nome);
                            DelRecUT(nome);
                            hash_table_DelU(nome);
                            UpdateAllMedia();
                            MessageBox(NULL,"UTENTE CANCELLATO", "SUCCESS!", MB_ICONERROR);
                        }

                        //---------------------DEBUG-------------
                        //print_hashUtenti();
                        //---------------------------------------
                    }
                    break;
                }
            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterDelUtentiClass(HINSTANCE HInst){
    WNDCLASSW DelUtenti = {0};
        DelUtenti.hbrBackground = (HBRUSH)COLOR_WINDOW;
        DelUtenti.hCursor = LoadCursor(NULL,IDC_ARROW);
        DelUtenti.hInstance = HInst;
        DelUtenti.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        DelUtenti.lpszClassName = L"myUtentiDelClass";
        DelUtenti.lpfnWndProc = UtentiDelProcedure;
	    RegisterClassW(&DelUtenti);
}

void displayDelUT(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myUtentiDelClass",(LPCWSTR)"Cancella Utente",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,300,200, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,30,50,50,20,HDLG,NULL,NULL,NULL);
    Hcancella = CreateWindowW(L"edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER,80,50,180,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Conferma",WS_VISIBLE | WS_CHILD,100,100,80,40,HDLG,(HMENU)DEL_UI_RI,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------


//Modal dialog ritoranti ↓

//ModalDialog Per l'aggiunta di ristoranti
//--------------------------------------------------------------------------

HWND HnomeR, Hcodiceid, Hlocalita, Htradizione, Hfasciaprezzo, Hmediavoti, Hprenotabili, Hchef;

LRESULT CALLBACK RistProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case AGG_RISTORANTE:
                {
                    char nome[20], localita[30], tradizione[20], chef[12], temp[10];
                    //temp ci serve per convertire la stringa in int
                    int codiceid = 0, fasciaprezzo = 0, mediavoti = 0, prenotabili = 0;
                    GetWindowText(HnomeR,nome,20);
				    GetWindowText(Hlocalita,localita,30);
                    GetWindowText(Hcodiceid,temp,9);
                    codiceid = atoi(temp);
                    GetWindowText(Hchef,chef,12);
				    GetWindowText(Htradizione,tradizione,20);
                    GetWindowText(Hfasciaprezzo,temp,5);
                    fasciaprezzo = atoi(temp);
				    GetWindowText(Hprenotabili,temp,2);
                    prenotabili = atoi(temp);
                    if(strcmp(nome, "")==0 || strcmp(localita, "")==0 || strcmp(tradizione, "")==0 || strcmp(chef, "")==0 || prenotabili==0 || codiceid == 0 || fasciaprezzo == 0){
                        MessageBox(NULL, "Inserisci i Valori Richiesti", " ERROR!", MB_ICONERROR);
                    }
                    else{
                        //------------------DEBUG------------
                        printf("nome : %s\ncodiceid : %d\nlocalita' : %s\ntradizione : %s\nfascia : %d\nmedia : %d\nposti prenotabili: %d\nchef %s\n", nome,codiceid,localita,tradizione,fasciaprezzo,mediavoti,prenotabili,chef);
                        //-----------------------------------
                        ristorante ri;
                        strcpy(ri.nome, nome);
                        strcpy(ri.localita, localita);
                        strcpy(ri.tradizione, tradizione);
                        strcpy(ri.chef, chef);
                        ri.codice_id = codiceid;
                        ri.p_prenotabili = prenotabili;
                        ri.fasciadiprezzo = fasciaprezzo;
                        ri.media_voti = 0;
                        if(hash_table_insertR(&ri)==1){
                            MessageBox(NULL, "INVALID DATA: IL RISTORANTE E' GIA' INSERITO", " ERROR!", MB_ICONSTOP);
                        }
                        else
                        {
                            //-----------------DEBUG-------------------
                            //print_hashRisto();
                            //-----------------------------------------
                            MessageBox(NULL, "Ristorante INSERITO!", " SUCCESS", MB_ICONEXCLAMATION);
                        }
                    }
                    break;
                }

            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterRistClass(HINSTANCE HInst){
    WNDCLASSW Rist = {0};
        Rist.hbrBackground = (HBRUSH)COLOR_WINDOW;
        Rist.hCursor = LoadCursor(NULL,IDC_ARROW);
        Rist.hInstance = HInst;
        Rist.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        Rist.lpszClassName = L"myRistClass";
        Rist.lpfnWndProc = RistoModProcedure;
	    RegisterClassW(&Rist);
}

void displayAggRI(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myRistClass",(LPCWSTR)"Nuovo Ristorante",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,425,400, hWnd, NULL, NULL, NULL);

    CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,55,50,50,20,HDLG,NULL,NULL,NULL);
    HnomeR = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,105,50,250,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"tradizione :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,85,80,20,HDLG,NULL,NULL,NULL);
    Htradizione = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,135,85,220,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"località :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,120,60,20,HDLG,NULL,NULL,NULL);
    Hlocalita = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,115,120,240,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"chef :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,155,40,20,HDLG,NULL,NULL,NULL);
    Hchef = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,95,155,165,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"codice id :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,190,80,20,HDLG,NULL,NULL,NULL);
    Hcodiceid = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,130,190,130,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"posti prenotabili :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,225,150,20,HDLG,NULL,NULL,NULL);
    Hprenotabili = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,225,90,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"fascia di prezzo :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,260,150,20,HDLG,NULL,NULL,NULL);
    Hfasciaprezzo = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,260,90,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Aggiungi",WS_VISIBLE | WS_CHILD,140,295,100,40,HDLG,(HMENU)AGG_RISTORANTE,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------

//ModalDialog Per la modifica di ristoranti
//--------------------------------------------------------------------------
LRESULT CALLBACK RistoModProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case CERCA_UT_RI:
                {
                    //--------------------DEBUG----------------
                    //print_hashUtenti();
                    //-----------------------------------------
                    char tmp[20];
                    GetWindowText(Hricerca, tmp, 20);
                    if(strcmp(tmp, "")==0)
                        MessageBox(NULL, "INSERISCI LA RICERCA", "ERROR!", MB_ICONERROR);
                    else{
                        //stringa minuscola in quanto nella hash le abbiamo minuscole
                        int ricerca = atoi(tmp);
                        ristorante *ri;
                        ri = hash_Table_LookRistorante(ricerca);
                        if(ri == NULL){
                            MessageBox(NULL, "RISTORANTE NON TROVATO", "ERROR!", MB_ICONERROR);
                        }
                        else
                        {
                            itoa(ri->codice_id, tmp, 20);
                            SetWindowText(Hcodiceid, tmp);
                            SetWindowText(HnomeR, ri->nome);
                            SetWindowText(Hlocalita, ri->localita);
                            SetWindowText(Htradizione, ri->tradizione);
                            SetWindowText(Hchef, ri->chef);
                            itoa(ri->fasciadiprezzo, tmp, 10);
                            SetWindowText(Hfasciaprezzo, tmp);
                            itoa(ri->p_prenotabili, tmp, 10);
                            SetWindowText(Hprenotabili, tmp);
                            EnableWindow(Hricerca, false);
                        }
                    }
                    break;
                }
                case AGG_RISTORANTE://riutilizzo lo stesso id siccome risparmio memoria non conviene occupare un altro id DEFINE
                {
                    char nome[20], localita[30], tradizione[20], chef[12], temp[10];
                    //temp ci serve per convertire la stringa in int
                    int codiceid = 0, fasciaprezzo = 0, mediavoti = 0, prenotabili = 0;
                    GetWindowText(Hricerca,temp,9);
                    codiceid = atoi(temp);
                    GetWindowText(HnomeR,nome,20);
				    GetWindowText(Hlocalita,localita,30);
                    GetWindowText(Hchef,chef,12);
				    GetWindowText(Htradizione,tradizione,20);
                    GetWindowText(Hfasciaprezzo,temp,5);
                    fasciaprezzo = atoi(temp);
				    GetWindowText(Hprenotabili,temp,2);
                    prenotabili = atoi(temp);
                    if(strcmp(nome, "")==0 || strcmp(localita, "")==0 || strcmp(tradizione, "")==0 || strcmp(chef, "")==0 || prenotabili==0 || codiceid == 0 || fasciaprezzo == 0){
                        MessageBox(NULL, "Inserisci i Valori Richiesti", " ERROR!", MB_ICONERROR);
                    }
                    else{
                        ristorante ri;
                        strcpy(ri.nome, nome);
                        strcpy(ri.localita, localita);
                        strcpy(ri.tradizione, tradizione);
                        strcpy(ri.chef, chef);
                        ri.codice_id = codiceid;
                        ri.p_prenotabili = prenotabili;
                        ri.fasciadiprezzo = fasciaprezzo;
                        ri.media_voti = 0;
                        hash_table_ModR(&ri);
                        MessageBox(NULL, "RISTORANTE MODIFICATO! Puoi effettuare un altra ricerca", "SUCCESS", MB_ICONEXCLAMATION);
                        //-----------------DEBUG-------------------
                        //print_hashUtenti();
                        //-----------------------------------------
                        //setto le box a null per averle vuote e riabilito la barra di ricerca
                        SetWindowText(HnomeR, NULL);
                        SetWindowText(Htradizione, NULL);
                        SetWindowText(Hfasciaprezzo, NULL);
                        SetWindowText(Hprenotabili, NULL);
                        SetWindowText(Hchef, NULL);
                        SetWindowText(Hlocalita, NULL);
                        EnableWindow(Hricerca, true);
                    }
                    break;
                }
            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterRIModClass(HINSTANCE HInst){
    WNDCLASSW RistoMod = {0};
        RistoMod.hbrBackground = (HBRUSH)COLOR_WINDOW;
        RistoMod.hCursor = LoadCursor(NULL,IDC_ARROW);
        RistoMod.hInstance = HInst;
        RistoMod.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        RistoMod.lpszClassName = L"myRistoModClass";
        RistoMod.lpfnWndProc = RistoModProcedure;
	    RegisterClassW(&RistoMod);
}


void displayModRI(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myRistoModClass",(LPCWSTR)"Modifica Ristorante",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,400,410, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"static",L"Codice id :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,75,30,75,20,HDLG,NULL,NULL,NULL);
    Hricerca = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER| ES_NUMBER,150,30,150,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Ricerca",WS_VISIBLE | WS_CHILD,140,60,80,40,HDLG,(HMENU)CERCA_UT_RI,NULL,NULL);

    CreateWindowW(L"static",L"Nome :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,110,50,20,HDLG,NULL,NULL,NULL);
    HnomeR = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,105,110,250,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"tradizione :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,145,80,20,HDLG,NULL,NULL,NULL);
    Htradizione = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,135,145,220,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"località :",WS_VISIBLE | WS_CHILD | WS_BORDER,55,180,60,20,HDLG,NULL,NULL,NULL);
    Hlocalita = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,115,180,240,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"chef :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,212,40,20,HDLG,NULL,NULL,NULL);
    Hchef = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER,95,212,165,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"posti prenotabili :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,245,150,20,HDLG,NULL,NULL,NULL);
    Hprenotabili = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,245,90,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"static",L"fascia di prezzo :",WS_VISIBLE | WS_CHILD | WS_BORDER ,55,280,150,20,HDLG,NULL,NULL,NULL);
    Hfasciaprezzo = CreateWindowW(L"edit", NULL,  WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,170,280,90,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Conferma",WS_VISIBLE | WS_CHILD,140,315,80,40,HDLG,(HMENU)AGG_RISTORANTE,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------

//ModalDialog Per la cancellazione di Ristoranti
//--------------------------------------------------------------------------
LRESULT CALLBACK RistorantiDelProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
                //da aggiungere CASE ...
                case DEL_UI_RI:
                {
                    char temp[10];
                    int codice_id = 0;
                    GetWindowText(Hcancella, temp, 10);
                    codice_id = atoi(temp);
                    if(strcmp(temp, "")==0)
                        MessageBox(NULL, "INSERISCI I VALORI", "ERROR!", MB_ICONERROR);
                    else
                    {
                        if(hash_Table_LookRistorante(codice_id)==NULL)
                        MessageBox(NULL,"RISTORANTE NON TROVATO", "ERROR!", MB_ICONERROR);
                        else
                        {
                            DelPreRI(codice_id);
                            DelRecRI(codice_id);
                            hash_table_DelR(codice_id);
                            MessageBox(NULL,"RISTORANTE CANCELLATO", "SUCCESS!", MB_ICONERROR);
                        }

                        //---------------------DEBUG-------------
                        //print_hashRisto();
                        //---------------------------------------
                    }
                    break;
                }
            }
            break;
        case WM_CLOSE:
            EnableWindow(HMainWindow, true);
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}

void RegisterDelRistoClass(HINSTANCE HInst){
    WNDCLASSW DelRisto = {0};
        DelRisto.hbrBackground = (HBRUSH)COLOR_WINDOW;
        DelRisto.hCursor = LoadCursor(NULL,IDC_ARROW);
        DelRisto.hInstance = HInst;
        DelRisto.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        DelRisto.lpszClassName = L"myRistoDelClass";
        DelRisto.lpfnWndProc = RistorantiDelProcedure;
	    RegisterClassW(&DelRisto);
}

void displayDelRI(HWND hWnd){
    HWND HDLG = CreateWindowW(L"myRistoDelClass",(LPCWSTR)"Cancella Ristorante",WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX&~WS_THICKFRAME|WS_VISIBLE ,500,400,300,200, hWnd, NULL, NULL, NULL);
    CreateWindowW(L"static",L"codice id :",WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER,30,50,50,20,HDLG,NULL,NULL,NULL);
    Hcancella = CreateWindowW(L"edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER| ES_NUMBER,80,50,180,20,HDLG,NULL,NULL,NULL);

    CreateWindowW(L"Button",L"Conferma",WS_VISIBLE | WS_CHILD,100,100,80,40,HDLG,(HMENU)DEL_UI_RI,NULL,NULL);
    EnableWindow(HMainWindow, false);//disabilitiamo la finestra in background per poi riattivarla
    //quando andiamo a chiudere questa
}
//--------------------------------------------------------------------------

//importa dai file i dati e aggiorna i corrispettivi dati
//------------------------------------------------------------------------------------------------------------------------
void IMPORT(){
FILE *Utenti_f, *Ristorante_f, *ptr_tmp;
if ((Utenti_f = fopen("./Dati/utenti.csv", "r")) == NULL || (Ristorante_f = fopen("./Dati/ristoranti.csv", "r")) == NULL)
    printf("Impossibile aprire il file.\n");
else{
    init_hash_table();

    //Lettura utenti
    while(!feof(Utenti_f))
    {
      utente ut;
      fscanf(Utenti_f,"%[^,],%[^,],%d,%[^,],%f,%f\n", ut.username, ut.email, &ut.n_anno, ut.tradizione, &ut.fasciadiprezzo, &ut.media_voti);
      hash_table_insertU(&ut);
    }
    fclose(Utenti_f);

    //Lettura Ristoranti
    while(!feof(Ristorante_f))
    {
      ristorante ri;
      fscanf(Ristorante_f,"%[^,],%d,%[^,],%[^,],%f,%f,%d,%s\n", ri.nome, &ri.codice_id, ri.localita, ri.tradizione, &ri.fasciadiprezzo,
      &ri.media_voti, &ri.p_prenotabili, ri.chef);
      hash_table_insertR(&ri);
    }
    fclose(Ristorante_f);

    typedef struct prenot{
        char nome[20]; int id_rest, giorno, mese, anno, posti_prenotati;
    }prenot;

    //Leggo le prenotazioni
    if((ptr_tmp = fopen("./Dati/prenotazioni.csv", "r")) == NULL)
        printf("file non trovato!\n");
    else{
        while(!feof(ptr_tmp)){
        prenot tmp;
        fscanf(ptr_tmp,"%[^,],%d,%d,%d,%d,%d\n", tmp.nome, &tmp.id_rest, &tmp.giorno,&tmp.mese, &tmp.anno, &tmp.posti_prenotati);

        utente *ut = hash_Table_LookUtente(tmp.nome); //Prendo l'utente tramite la prenotazione

        //Inserisco la prenotazione nell'utente
            if(ut != NULL)
              insertPre(ut, tmp.nome, tmp.id_rest, tmp.giorno, tmp.mese, tmp.anno, tmp.posti_prenotati);
            }
        }
        fclose(ptr_tmp);

        //------------------DEBUG------------------
        //print_hashUtenti();
        //print_hashRisto();
        //-----------------------------------------

        //Stampa le prenotazioni
        //printAllPre();

        //Va ad aggiornare i posti prenotabili in base a chi ha già prenotato

        //------------------DEBUG------------------
        //print_hashRisto();
        //-----------------------------------------

        typedef struct tmp_r{
                char nome[20]; int id_rest; int val;
        }rist;

        if((ptr_tmp = fopen("./Dati/recensioni.csv", "r")) == NULL)
            printf("file non trovato!\n");
        else{
            while(!feof(ptr_tmp)){
                rist tmp;
                fscanf(ptr_tmp,"%[^,],%d,%d\n", tmp.nome, &tmp.id_rest, &tmp.val);

                utente *ut = hash_Table_LookUtente(tmp.nome);

                if(ut != NULL){
                    insertRec(ut, tmp.nome, tmp.id_rest, tmp.val);
                }
            }
        }
        fclose(ptr_tmp);

        //------------------DEBUG------------------
        //printf("NOT CRASH!\n");
        //printAllRec();
        //-----------------------------------------

        UpdateAllMedia();

        //-----------------DEBUG-------------------
        //print_hashRisto();
        //-----------------------------------------

        UpdateAllUserMedia();

        //-----------------DEBUG-------------------
        //print_hashUtenti();
        //-----------------------------------------
    }
}
//-------------------------------------------------------------------------------------------------------------------------

//registri e procedure della finestra child nel main "nuova prenotazione"
//se facessi solo una static box creerebbe un bug grafico visivo nel il bordo degli elementi all interno non sarebbero visibili
//----------------------------------------------------------------------------------------
void RegisterNullClass(HINSTANCE HInst){
    WNDCLASSW Null = {0};
        Null.hbrBackground = (HBRUSH)COLOR_WINDOW;
        Null.hCursor = LoadCursor(NULL,IDC_ARROW);
        Null.hInstance = HInst;
        Null.hIcon = (HICON)LoadImage(NULL, "c_lJX_icon.ico",IMAGE_ICON, 32,32,LR_LOADFROMFILE);
        Null.lpszClassName = L"myNullClass";
        Null.lpfnWndProc = NullProcedure;
	    RegisterClassW(&Null);
}

LRESULT CALLBACK NullProcedure(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp){
    switch(msg){
        case WM_COMMAND:
            switch(wp){
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        default:
                return DefWindowProc(hWnd,msg,wp,lp);
    }
}
//---------------------------------------------------------------------------------------------------------





//Cerca L'utente (per il main)
//-------------------------------------------------------------------------------------------------------------------------
int Cerca_utM(HWND Hricerca)
{
    char ricerca[20];
    GetWindowText(Hricerca, ricerca, 20);
    if(strcmp(ricerca, "") == 0)
        MessageBox(NULL, "INSERISCI LA RICERCA", "ERROR!", MB_ICONERROR);
    else{
        //stringa minuscola in quanto nella hash le abbiamo minuscole
        toLower(ricerca);
        utente *ut;
        ut = hash_Table_LookUtente(ricerca);
        if(ut == NULL){
            MessageBox(NULL, "UTENTE NON TROVATO", "ERROR!", MB_ICONERROR);
        }
        else
        {
            MessageBox(NULL, "UTENTE TROVATO!", "SUCCESS!", MB_ICONINFORMATION);
            return 1;
        }
    }
    return 0;
}
//-------------------------------------------------------------------------------------------------------------------------

//funzione che riabilita tutte le finestre del main, occuperebbero troppo spazio nel main e si creerebbe molta confusione
//---------------------------------------------------------------------
void EnableAllWindow(HWND hWnd)
{
    //prenotazione enable
    EnableWindow(hbutton, true);
    EnableWindow(HPreGio, true);
    EnableWindow(HPreMese, true);
    EnableWindow(HPreAnno, true);
    EnableWindow(HPreid, true);
    EnableWindow(hTBid, true);
    EnableWindow(hTBpre, true);
    EnableWindow(hPPreb, true);
    EnableWindow(hPPre, true);
    
    //recensione enable
    EnableWindow(hRecIDb, true);
    EnableWindow(hRecID, true);
    EnableWindow(hRecVotob, true);
    EnableWindow(hRecVoto, true);
    EnableWindow(hRecBut, true);

    //annulla pre enable
    EnableWindow(hCPreIDb, true);
    EnableWindow(hCPreID, true);
    EnableWindow(hCPreGb, true);
    EnableWindow(hCPreG, true);
    EnableWindow(hCPreMb, true);
    EnableWindow(hCPreM, true);
    EnableWindow(hCPreBut, true);

    //modifica pre enable
    EnableWindow(hMPreIDb, true);
    EnableWindow(hMPreID, true);
    EnableWindow(hMPreGb, true);
    EnableWindow(hMPreG, true);
    EnableWindow(hMPreMb, true);
    EnableWindow(hMPreM, true);
    EnableWindow(hMPreBut, true);
    EnableWindow(hMPreAb, true);
    EnableWindow(hMPreA, true);
    EnableWindow(hMPPre, true);
    EnableWindow(hMPPreb, true);

    //ricerca ri enable
    EnableWindow(hComboBox, true);
    EnableWindow(hRRic, true);
    EnableWindow(hRecb, true);
    EnableWindow(hRecTextb, true);
}
//----------------------------------------------------------------------

//funzione che cancella i dati immessi nelle window dall utente precedente
//---------------------------------------------------------------------
void DeleteAllWindow()
{
    //prenotazione enable
    SetWindowText(HPreGio, "");
    SetWindowText(HPreMese, "");
    SetWindowText(HPreAnno, "");
    SetWindowText(HPreid, "");
    SetWindowText(hPPre, "");

    
    //recensione enable
    SetWindowText(hRecID, "");
    SetWindowText(hRecVoto, "");

    //annulla pre enable
    SetWindowText(hCPreID, "");
    SetWindowText(hCPreG, "");
    SetWindowText(hCPreM, "");
    
    //modifica pre enable
    SetWindowText(hMPreID, "");
    SetWindowText(hMPreG, "");
    SetWindowText(hMPreM, "");
    SetWindowText(hMPreA, "");
    SetWindowText(hMPPre, "");

    //ricerca ri enable
    SetWindowText(hRRic, "");
    SetWindowText(hRecTextb, "");
}

//funzione tempo
//----------------------------------------------------------------------
struct tm getime()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm.tm_year += 1900;
    tm.tm_mon += 1;
    return tm;
}

//controlla se è un numero
bool checkdigit(char ricerca[])
{
    int len = strlen(ricerca);
    int rval;
    bool entered = false;
    for(int i = 0; i<len; i++)
    {
        rval = isdigit(ricerca[i]);
        if(rval == 0)
            entered = true;
    }
        if(entered == true)
            return false;
        else
            return true;
}

//controlla se è un numero + float
bool checkdigitf(char ricerca[])
{
    int len = strlen(ricerca);
    int rval, counterpoint = 0;
    bool entered = false;
    for(int i = 0; i<len; i++)
    {
        rval = isdigit(ricerca[i]);
        if(rval == 0 && ricerca[i] != '.')
            entered = true;
        else
        {
            if(ricerca[i] == '.')
                counterpoint += 1;
        }
    }
    if(counterpoint>1){
        MessageBox(NULL, "INSERISCI DATI CORRETTI", "ERROR!", MB_ICONERROR);
    }
    else
        if(entered == true)
            return false;
        else
            return true;
}

//controlla se è un carattere alfanumerico
bool checkalfa(char ricerca[])
{
    int len = strlen(ricerca);
    int rval;
    bool entered = false;
    for(int i = 0; i<len; i++)
    {
        rval = isalpha(ricerca[i]);
        if(rval == 0)
            entered = true;
    }
        if(entered == true)
            return false;
        else
            return true;
}

//esporta i dati
void EXPORT()
{
    FILE *Utenti_f, *Ristorante_f, *ptr_tmp;
    if ((Utenti_f = fopen("./Dati/utenti.csv", "w")) == NULL || (Ristorante_f = fopen("./Dati/ristoranti.csv", "w")) == NULL)
        printf("Impossibile aprire il file.\n");
    else{     
        int i = 0;
        //scrittura utenti
        while(!feof(Utenti_f) && i<TABLE_SIZE)
        {
            if(hash_table_utenti[i]!=NULL)
            fprintf(Utenti_f,"%s,%s,%d,%s,%f,%f\n", hash_table_utenti[i]->username, hash_table_utenti[i]->email, hash_table_utenti[i]->n_anno, hash_table_utenti[i]->tradizione, hash_table_utenti[i]->fasciadiprezzo, hash_table_utenti[i]->media_voti);
            i++;
        }
        fclose(Utenti_f);

    
        i = 0;
        //scrittura Ristoranti
        while(!feof(Ristorante_f) && i<TABLE_SIZE)
        {
            if(hash_table_ristoranti[i]!=NULL)
                fprintf(Ristorante_f,"%s,%d,%s,%s,%f,%f,%d,%s\n", hash_table_ristoranti[i]->nome, hash_table_ristoranti[i]->codice_id, hash_table_ristoranti[i]->localita, hash_table_ristoranti[i]->tradizione, hash_table_ristoranti[i]->fasciadiprezzo,
                hash_table_ristoranti[i]->media_voti, hash_table_ristoranti[i]->p_prenotabili, hash_table_ristoranti[i]->chef);
            i++;
        }
        fclose(Ristorante_f);

        i = 0;
        //scrittura prenotazioni
        if((ptr_tmp = fopen("./Dati/prenotazioni.csv", "w")) == NULL)
            printf("file non trovato!\n");
        else
        {
            while(!feof(ptr_tmp) && i<TABLE_SIZE){
                if(hash_table_utenti[i]!=NULL){
                    utente *ut = hash_Table_LookUtente(hash_table_utenti[i]->username);
                    prenotazione *tmp = hash_table_utenti[i]->p_prenotabili;
                    while(tmp!=NULL){
                        fprintf(ptr_tmp,"%s,%d,%d,%d,%d,%d\n", tmp->username, tmp->id_rest, tmp->giorno,tmp->mese, tmp->anno, tmp->posti_prenotati);
                        tmp = tmp->next;
                    }
                }
                i++;
            }
        }
        fclose(ptr_tmp);

        i = 0;
        //scrittura recensione
        if((ptr_tmp = fopen("./Dati/recensioni.csv", "w")) == NULL)
            printf("file non trovato!\n");
        else{
            while(!feof(ptr_tmp) && i<TABLE_SIZE){
                if(hash_table_utenti[i]!=NULL){
                    utente *ut = hash_Table_LookUtente(hash_table_utenti[i]->username);
                    recensione *tmp = hash_table_utenti[i]->valutazioni;
                    while(tmp!=NULL){
                        fprintf(ptr_tmp,"%s,%d,%d\n", tmp->username, tmp->id_rest, tmp->val);
                        tmp = tmp->next;
                    }
                }
                i++;
            }
        }
        fclose(ptr_tmp);
    }
}