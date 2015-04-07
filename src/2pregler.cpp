/**
 *  @package   2pregler
 *  @file      2pregler.cpp
 *  @brief     Zweipunktregler fuer eine Heizung   
 *  @author    Rolf Hemmerling <hemmerling@gmx.net>
 *  @version   1.00, 
 *             Programmiersprache "Borland C/C++" fuer MSDOS,
 *             Entwicklungswerkzeuge 
 *                                   - TURBO C/C++ 2.0 / 3.0 
 *                                   - Borland C/C++ 3.1
 *                                   - Turbo C/C++ 4.5
 *  @date      2015-01-01
 *  @copyright Apache License, Version 2.0
 *
 *  2pregler.cpp - Zweipunktregler fuer eine Heizung 
 *                 Labor fuer Datenverarbeitung, FH Hannover,
 *                 Versuch 3 "Zweipunktregler"   
 *  
 *  Copyright 2000-2015 Rolf Hemmerling
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an
 *  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 *  either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *  Diese Software laesst sich nicht mit dem freien 
 *  Entwicklungswerkzeug "Borland C/C++ 5.5" uebersetzen, 
 *  da dieses nur Windows- und keine DOS-Programme erzeugt 
 *  ( u.a. ist die C-Structure REGS unbekannt ).
 *
 *  Letztes Update: 2000-04-07
 */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Include-Dateien                                                 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "2pregler.h"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Modulglobale Variablen                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @var      MV_kaldaten
 *  @brief    Feld mit Daten der Eichkurve
 */
unsigned int MV_kaldaten[2][101];
/*******************************************************************/
/* Feld mit Daten der Eichkurve                                    */
/* [0][0] = Totzeit T_aus, ein Zaehlwert                           */
/* [0][1] = Totzeit T_ein, ein Zaehlwert                           */
/* [0][?] = Temperatur in ¯C                                       */
/* [1][?] = dazu ermittelter Zaehlerwert                           */
/* [?][101] = Bis zu 100 Kalibrierungdaten erlaubt                 */
/*******************************************************************/

/**
 *  @var      MV_anzahl
 *  @brief    Anzahl der Eichpunkte
 */
unsigned int MV_anzahl;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Funktions-Deklarationen                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void MP_cursor_entfernen(void)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Der Cursor wird in einen nicht sichtbaren Bereich gesetzt.     */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
   union REGS PV_inregs,PV_outregs;
   PV_inregs.h.ah = 0x02; /* Funktion 2, Cursor setzen */
   PV_inregs.h.bh = 0x00; /* aktuelle Bildschirmseite */
   PV_inregs.h.dh = 0x25; /* y-Position = 25 */
   PV_inregs.h.dl = 0x0;  /* x-Position =  0 */
   /* Bios-Interupt 10h aufrufen */
   int86 (0x10, &PV_inregs, &PV_outregs);
 }


void MP_datumzeit(char VV_xx, char VV_yy,
                  char VV_art, char VV_wann)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Datum,Zeit anzeigen                                            */
/* Parameter:                                                      */
/*  VV_x = Position x                                              */
/*  VV_y = Position y                                              */
/*  VV_art = 1 = nur Datum anzeigen                                */
/*           2 = nur Zeit anzeigen                                 */
/*           3 = beides anzeigen                                   */
/*  VV_wann  MC_tsofort = neu anzeigen, auch wenn noch keine       */
/*                        Sekunde vergangen ist                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 static struct  date MV_datum; /* statisch definieren */
 static struct  time MV_zeit;  /* da sonst Werte beim Verlassen */
 static char    MV_sec;        /* der Funktion verloren gehen */
 char           PV_mode;
 char           PV_buf[20];
 unsigned int PV_maxx,PV_maxy,PV_xpos,PV_ypos;

 /* alte Sekunde sichern */
 MV_sec=MV_zeit.ti_sec;

 /* aktuelle Zeit einlesen */
 gettime(&MV_zeit);    

 if(MV_sec!=MV_zeit.ti_sec || VV_wann==MC_tsofort)
   {
     /* wenn schon 1 Sec. vergangen ist oder sofort angezeigt werden soll */
     PV_mode=getgraphmode();

     /* Datum holen */
     getdate(&MV_datum);     
     gotoxy(VV_xx,VV_yy);

     /* soll Datum ausgegeben werden? */
     if(VV_art!=MC_tzeit)
      {
        if(PV_mode<0 || PV_mode>5)
         {
           printf("Datum: %2d.%2d.%d",MV_datum.da_day,
                  MV_datum.da_mon,MV_datum.da_year);
         }
        else
         {
           PV_xpos=getx();
           PV_ypos=gety();
           PV_maxx=getmaxx()+1;
           PV_maxy=getmaxy()+1;
           sprintf(PV_buf,"Datum: %2d.%2d.%d",MV_datum.da_day,
                   MV_datum.da_mon,MV_datum.da_year);
           setfillstyle(SOLID_FILL,BLACK);
           bar((PV_maxx/80)*VV_xx,(PV_maxy/25)*VV_yy,
               (PV_maxx/80)*VV_xx+textwidth(PV_buf),
               (PV_maxy/25)*VV_yy+textheight(PV_buf));
	   outtextxy((PV_maxx/80)*VV_xx,(PV_maxy/25)*VV_yy,PV_buf);
           moveto(PV_xpos,PV_ypos);
         }
	VV_yy++;   /*  Y-Wert auf nÑchste Zeile setzen */
      }
     gotoxy(VV_xx,VV_yy);

     if(VV_art!=MC_tdatum)  /* soll Zeit ausgegeben werden? */
      {
        if(PV_mode<0 || PV_mode>5)
         {
           printf("Zeit : %2d:%2d:%2d",MV_zeit.ti_hour,
                  MV_zeit.ti_min,MV_zeit.ti_sec);
         }
        else
         {
           PV_xpos=getx();
           PV_ypos=gety();
           sprintf(PV_buf,"Zeit : %2d:%2d:%2d",MV_zeit.ti_hour,
                   MV_zeit.ti_min,MV_zeit.ti_sec);
           setfillstyle(SOLID_FILL,BLACK);
	   bar((PV_maxx/80)*VV_xx,(PV_maxy/25)*VV_yy,
	       (PV_maxx/80)*VV_xx+textwidth(PV_buf),
	       (PV_maxy/25)*VV_yy+textheight(PV_buf));
	   outtextxy((PV_maxx/80)*VV_xx,(PV_maxy/25)*VV_yy,PV_buf);
	   moveto(PV_xpos,PV_ypos);
         }
      }

     /* Cursor in nicht sichtbaren Bereich legen */
     MP_cursor_entfernen(); 
   }
 }


FILE * MP_laden(void)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Datei laden                                                    */
/* Parameter: -                                                    */
/* Rueckgabewert:                                                  */
/*  FILE * = Dateizeiger                                           */
/*******************************************************************/
 {
 FILE *PV_fp;
 char PV_datei[200];

 clrscr();
 printf("\n\n\n\n      DATEI ôFFNEN");
 printf("\n      ============");
 printf("\n\n\n      Bitte geben Sie den Dateinamen ein: ");

 MP_datumzeit(60,1,MC_tbeides,MC_tsofort);
 gotoxy(43,9);

 scanf("%s",PV_datei);

 /* Datei vorhanden? */
 if((PV_fp=fopen(PV_datei,"rt"))==NULL)   
   {
     /* wenn nicht Fehlermeldung */
     clrscr();
     printf("\n\n\n\n      ACHTUNG!");
     printf("\n      ========");
     printf("\n\n\n      Datei nicht gefunden.");
     printf("\n      Bitte ÅueberprÅfen Sie Dateiname und Pfadangabe.");
     printf("\n\n\n      Weiter mit Taste.");
     MP_datumzeit(60,1,MC_tbeides,0);
     do
      {
        MP_datumzeit(60,1,MC_tbeides,0);
      }
     while(!kbhit());
   }
  return(PV_fp);
 }


FILE * MP_speichern(void)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Datei speichern                                                */
/* Parameter: -                                                    */
/* Rueckgabewert:                                                  */
/*  FILE * = Dateizeiger                                           */
/*******************************************************************/
{
 FILE *PV_fp;
 char PV_datei[200];
 char PV_key;

 clrscr();
 printf("\n\n\n\n      DATEI SPEICHERN");
 printf("\n      ===============");
 printf("\n\n\n      Bitte geben Sie den Dateinamen ein: ");

 MP_datumzeit(60,1,MC_tbeides,MC_tsofort);
 gotoxy(43,9);

 scanf("%s",PV_datei);

 PV_fp=fopen(PV_datei,"rt");
 if(PV_fp!=NULL)
   {
   fclose(PV_fp);
   PV_fp=NULL;
   clrscr();
   printf("\n\n\n\n      ACHTUNG!");
   printf("\n      ========");
   printf("\n\n\n      Datei existiert schon.");
   printf("\n      Soll die Datei Åberschrieben werden (j/n)? ");
   MP_datumzeit(60,1,MC_tbeides,0);
   do
    {
    MP_datumzeit(60,1,MC_tbeides,0);
    }
   while(!kbhit());
   PV_key=getch();
   }

 if(PV_fp==NULL || PV_key=='j' || PV_key=='J')
   PV_fp=fopen(PV_datei,"wt");
 return(PV_fp);

 }


void MP_eichung()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Eichkurve aufnehmen                                            */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 unsigned int PV_zaehler;
 unsigned int PV_min,PV_max,PV_step;
 unsigned int LV_ii,PV_temp;
 char           PV_fehler,PV_key;
 FILE           *PV_fp;

 do
  {
  clrscr();
  printf(
"\n\n      EICHKURVE AUFNEHMEN");
  printf(
"\n      ===================");
  printf(
"\n\n   Hiermit wird eine Eichkurve aufgenommen.");
  printf(
"\n   Geben Sie zuerst die Starttemperatur, danach die Endtemperatur");
  printf(
"\n   und zum Schlu· die Schrittweite ein. Die eingegebenen Werte");
  printf(
"\n   mÅssen ganzzahlig sei.");
  printf(
"\n   Es mueÅssen sich zwischen 10 und 100 Messwerte ergeben.");
  printf(
"\n   Die Starttemperatur sollte groesser sein, als die aktuelle Temperatur.");
  printf(
"\n\n   Waehrend der Messung muss eine Taste gedrÅckt werden, wenn die");
  printf(
"\n   aktuelle Temperatur den auf dem Bildschirm vorgegebenen Wert");
  printf(
"\n   erreicht hat.");
  printf(
"\n\n   Starttemperatur in Grad Celsius:");
  printf(
"\n\n   Endtemperatur in ¯C  :");
  printf(
"\n\n   Schrittweite in ¯C   :");

  fflush(stdin);
  gotoxy(27,17);
  scanf("%3d",&PV_min);

  fflush(stdin);
  gotoxy(27,19);
  scanf("%3d",&PV_max);

  fflush(stdin);
  gotoxy(27,21);
  scanf("%3d",&PV_step);

  fflush(stdin);
  PV_fehler=MC_false;

  if(PV_step<1)
    {
    printf("\n   Achtung! Die Schrittweite ist kleiner als 1.");
    PV_fehler=MC_true;
    }
  else
    {
    if((PV_max-PV_min)/PV_step>100)
      {
      printf("\n   Achtung! Es ergeben sich mehr als 100 Me·werte.");
      PV_fehler=MC_true;
      }
    if((PV_max-PV_min)/PV_step<10)
      {
      printf("\n   Achtung! Es ergeben sich weniger als 10 Me·werte.");
      PV_fehler=MC_true;
      }
    }
  if (PV_fehler==MC_true) printf("\n   Weiter mit Taste...");
  else printf("\n\n   Start der Messung mit Taste...");
  getch();
  }
 while (PV_fehler==MC_true);

 /* Anzahl der Me·werte */
 MV_anzahl=(PV_max-PV_min)/PV_step+1;
 clrscr();
 PV_fehler=MP_graphik_ein();

 if(PV_fehler==MC_false)
   {
   /* Heizung ein */
   MP_portausgabe(MC_portaddr,1,1);
   setcolor(WHITE);
   setfillstyle(SOLID_FILL,MC_off_colour);
   outtextxy(10,10,"Abbruch mit ESC");
   for(LV_ii=1;LV_ii<=MV_anzahl;LV_ii++)
      {
      /* gewÅnschte Temp. berechnen */
      PV_temp=PV_min+(LV_ii-1)*PV_step;
      fflush(stdin);
      /* Temp. anzeigen */
      MP_led_anzeige (PV_temp/100, 100,170,2);
      MP_led_anzeige ((PV_temp%100)/10, 170,170,2);
      MP_led_anzeige (PV_temp%10,240,170,2);
      MP_led_anzeige (11, 330,170,1);
      MP_led_anzeige (10, 380,170,2);
      do
       {
       setcolor(WHITE);
       MP_datumzeit(60,1,MC_tbeides,0);
       }
      while(!kbhit()); /* Auf Taste warten */

      PV_key=getch();

      if(PV_key==MC_kbesc)
        {
        MV_anzahl=0;
        PV_fehler=MC_true;
        /* Wenn ESC aus Schleife springen */
        break;                                          
        }

      /* Monoflop antriggern */
      MP_portausgabe(MC_portaddr+2,0,0);
      MP_portausgabe(MC_portaddr+2,0,1);
      MP_portausgabe(MC_portaddr+2,0,0);
      PV_zaehler=MP_timer();

      /* Temperatur in ¯C speichern */
      MV_kaldaten[0][LV_ii]=PV_temp;
      /* ZÑhlerwert dazu  speichern */
      MV_kaldaten[1][LV_ii]=PV_zaehler;
      }

   /* Heizung aus */
   MP_portausgabe(MC_portaddr,1,0);
   MP_graphik_aus();

   if(PV_fehler==MC_false)
     {
     MP_totzeit();
     printf("\n\n\n   Messung der Kalibrierkurve beendet.");
     printf("\n\n\n   S) Eichkurve speichern");
     printf("\n ESC) ZurÅck zum HauptmenÅ");
     do
      {
      if(kbhit()) PV_key=getch();
      MP_datumzeit(60,1,MC_tbeides,0);
      }
     while(PV_key!=MC_kbesc && PV_key!='s' && PV_key!='S');
     if(PV_key=='s' || PV_key=='S')
       {
       /* Kurve speichern */
       PV_fp=MP_speichern();                                  
       if(PV_fp!=NULL)
	{
        fprintf(PV_fp,"T_aus: %ld ms\n",
                (unsigned long)MV_kaldaten[0][0]*100);
        fprintf(PV_fp,"T_an : %ld ms\n\n",
                (unsigned long)MV_kaldaten[1][0]*100);
        fprintf(PV_fp,"Temperatur    ZÑhlerwert\n");
	for(LV_ii=1;LV_ii<=MV_anzahl;LV_ii++)
	   fprintf(PV_fp,"  %3d ¯C        %6d\n",
		   MV_kaldaten[0][LV_ii],MV_kaldaten[1][LV_ii]);
        fclose(PV_fp);
        }
       }
     }
   }
 }


void MP_kurve_laden()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Eichkurve laden                                                */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 FILE          *PV_fp;
 unsigned long PV_dummy;
 char          PV_ii;

 PV_fp=MP_laden();
 if(PV_fp!=NULL)
   {
   fscanf(PV_fp,"T_aus: %ld ms\n",&PV_dummy);
   MV_kaldaten[0][0]=PV_dummy/100;
   fscanf(PV_fp,"T_an : %ld ms\n\n",&PV_dummy);
   MV_kaldaten[1][0]=PV_dummy/100;
   fscanf(PV_fp,"Temperatur    ZÑhlerwert\n");
   PV_ii=1;
   while(fscanf(PV_fp,"  %3d ¯C        %6d\n",
         &MV_kaldaten[0][PV_ii],&MV_kaldaten[1][PV_ii])!=-1)
        PV_ii++;
   MV_anzahl=PV_ii-1;
   fclose(PV_fp);
   printf("\n\n\n   Kalibrierkurve erfolgreich geladen.");
   printf("\n   Weiter mit Taste...");
   getch();
   }
 }


void MP_anzeigen()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Eichkurve anzeigen                                             */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char           PV_fehler,LV_ii,PV_key;
 double         PV_xpunkt,PV_ypunkt;
 unsigned int PV_dx,PV_dy;
 unsigned int PV_xmin,PV_ymin;
 unsigned int PV_xpunktmin,PV_ypunktmin;

 if (MV_anzahl>0)
    {
    PV_fehler=MP_graphik_ein();
    if(PV_fehler==MC_false)
      {

      /* Berechnungen fÅr X-Achse */
      PV_dx=(MV_kaldaten[0][MV_anzahl]-MV_kaldaten[0][1])/8/5*5+5;

      /* Differenz zwischen zwei Hauptintervallen */
      PV_xmin=MV_kaldaten[0][1]/PV_dx*PV_dx;

      /* kleinster Wert an Achse */
      PV_xpunkt=5*11/(double)PV_dx;

      /* Werte pro Pixel */
      /* Wenn Skala bei 0 anfÑngt, bei Pixel 59 beginnen, sonst bei 114 */
      if(PV_xmin==0)
       {
         PV_xpunktmin=59;
       }
      else
       {
         PV_xpunktmin=114;
       };

      /* Berechnungen fÅr Y-Achse */
      PV_dy=(MV_kaldaten[1][1]-MV_kaldaten[1][MV_anzahl])/8/500*500+500;

      /* Differenz zwischen zwei Hauptintervallen */
      PV_ymin=MV_kaldaten[1][MV_anzahl]/PV_dy*PV_dy;

      /* kleinster Wert an Achse */
      PV_ypunkt=5*9/(double)PV_dy;

      /* Werte pro Pixel */
      /* Wenn Skala bei 0 anfÑngt, bei Pixel 450 beginnen */
       /* sonst bei 405 */
      if(PV_ymin==0)
       {
         PV_ypunktmin=450;
       }
      else
       {
         PV_ypunktmin=405;
       };

      MP_graph_zeichnen(PV_xmin,PV_dx,PV_ymin,PV_dy,"T","¯C","ZÑhler","");

      /* Koordinatensystem zeichnen, Cursor auf 1. Punkt setzen */
      moveto(PV_xpunktmin+(MV_kaldaten[0][1]-PV_xmin)*PV_xpunkt,
             PV_ypunktmin-(MV_kaldaten[1][1]-PV_ymin)*PV_ypunkt);

      /* Alle weiteren Punkte zeichnen */
      for(LV_ii=1;LV_ii<=MV_anzahl;LV_ii++)
       {
         lineto(PV_xpunktmin+(MV_kaldaten[0][LV_ii]-PV_xmin)*PV_xpunkt,
		PV_ypunktmin-(MV_kaldaten[1][LV_ii]-PV_ymin)*PV_ypunkt);
       };

      textbackground(BLACK);

      do
       {
         if(kbhit()) PV_key=getch();
         /* Datum und Zeit anzeigen */
         MP_datumzeit(60,1,MC_tbeides,0);
       }
      while(PV_key!=MC_kbesc); /* Auf ESC warten */

      MP_graphik_aus();
      }
    }
  else
   {
   clrscr();
   printf("\n\n\n\n   Noch keine Kalibrierdaten vorhanden.");
   printf("\n   Bitte erst Eichkurve aufnehmen oder laden.");
   printf("\n\n   Weiter mit Taste...");
   getch();
   }
 }


void MP_regler()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  RegelermenÅue                                                   */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char PV_key;
 if (MV_anzahl>0)
    {
    do
     {
     clrscr();
     PV_key=' ';
     printf("\n\n\n\n      REGLER  AUSWAHL");
     printf("\n      ===============");
     printf("\n\n\n   1) statische Grenze");
     printf("\n   2) dynamische Grenzen");
     printf("\n ESC) Auswahl abbrechen");
     printf("\n\n Bitte waehlen Sie: ");
     MP_datumzeit(60,1,MC_tbeides,MC_tsofort);

     do
      {
      if(kbhit()) PV_key=getch();
      MP_datumzeit(60,1,MC_tbeides,0);
      }
     while(PV_key!=MC_kbesc && PV_key!='1' && PV_key!='2');

     if(PV_key=='1')
       MP_stat_regler();
     if(PV_key=='2')
       MP_dyn_regler();

     }
    while(PV_key!=MC_kbesc);
    }
 else
   {
   clrscr();
   printf("\n\n\n\n   Noch keine Kalibrierdaten vorhanden.");
   printf("\n   Bitte erst Eichkurve aufnehmen oder laden.");
   printf("\n\n   Weiter mit Taste...");
   getch();
   }
 }

unsigned int MP_timer()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Timer                                                          */
/* Parameter: -                                                    */
/* Rueckgabewert:                                                  */
/*  int Zaehlerwert                                                */
/*******************************************************************/

 {
 unsigned int PV_zaehler;

 /* ZÑhler in Assembler */
 asm{
           push ax       // Reg. sichern
           push cx
           push dx
           pushf         // Statusflags sichern
           mov  cx,0     // Reg. CX auf 0 setzen
           mov  dx, MC_portaddr  // Portaddr. in DX
           inc  dx       // einen erhîhen fÅr Statusreg.
           cli           // Interrupts sperren
     }
  ML_loop:                  // Schleifenanfang
  asm{
           inc  cx       // CX hochzÑhlen
           in   al,dx    // Port auslesen
           and  al,128   // Bit 7 maskieren
           jnz ende      // Wenn kein Signal mehr, Schleifenende
           jmp ML_loop   //  sonst nochmal
     }
  ende:
  asm{
           mov [PV_zaehler],cx  // ZÑhlerwert speichern
           sti           // Interrups einschalten
           popf          // Statusflags wiederherstellen
           pop dx        // Reg. wiederherstellen
           pop cx
           pop ax
     }
 return(PV_zaehler);        // ZÑhlerwert zurÅckgeben
 }


char MP_graphik_ein()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Grafik einschalten                                             */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int  PV_graphdriver, PV_graphmode;
 char PV_fehler=MC_false;
 char result;

 /* Grafikmodus bestimmen */
 detectgraph(&PV_graphdriver,&PV_graphmode);

 /* Grafikmodus setzen */
 initgraph (&PV_graphdriver,&PV_graphmode, MC_treiberpfad);

 /* Fehler abfragen */
 result=graphresult();                                

 if (result==grOk)
   {
     /* Wenn kein Fehler, OK */
     cleardevice();
   }
 else
  {
  /*   sonst Fehlermeldung */
  clrscr();
  printf("\n\n\n   Fehler beim Wechseln in den Grafikmodus.");
  printf("\n\n   Fehlermeldung: %s",grapherrormsg(result));
  printf("\n\n   Weiter mit Taste...");
  getch();
  PV_fehler=MC_true;
  }
 return(PV_fehler);
 }


void MP_graphik_aus()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Grafik ausschalten                                             */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
  /* Grafikmodus ausschalten */
  closegraph();
 }



void MP_led_anzeige(unsigned int VV_zeichen,
                    unsigned int VV_xx,
                    unsigned int VV_yy, float VV_kk)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  LED Anzeige - 14-Segment-Anzeige definieren                    */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int PV_segment;
 int   PV_border;
 int PV_segpunkte[14]=
       {5,5,5,5,5,5,6,6,7,5,7,7,5,7};
 /* Anzahl der Punkte im Segment */

 int PV_punktfeld[14][16]=
     {{( 2*VV_kk)+VV_xx,( 1*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,
       ( 1*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,( 5*VV_kk)+VV_yy,
       ( 6*VV_kk)+VV_xx,( 5*VV_kk)+VV_yy,( 2*VV_kk)+VV_xx,
       ( 1*VV_kk)+VV_yy,0,0,0,0,
       (10*VV_kk)+VV_xx,( 3*VV_kk)+VV_yy},
      {(27*VV_kk)+VV_xx,( 2*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       (26*VV_kk)+VV_yy,(23*VV_kk)+VV_xx,(24*VV_kk)+VV_yy,
       (23*VV_kk)+VV_xx,( 6*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       ( 2*VV_kk)+VV_yy,0,0,0,0,
       (25*VV_kk)+VV_xx,(15*VV_kk)+VV_yy},
      {(23*VV_kk)+VV_xx,(30*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,
       (28*VV_kk)+VV_yy,(27*VV_kk)+VV_xx,(51*VV_kk)+VV_yy,
       (23*VV_kk)+VV_xx,(47*VV_kk)+VV_yy,(23*VV_kk)+VV_xx,
       (30*VV_kk)+VV_yy,0,0,0,0,
       (25*VV_kk)+VV_xx,(38*VV_kk)+VV_yy},
      {( 6*VV_kk)+VV_xx,(48*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,
       (48*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,(52*VV_kk)+VV_yy,
       ( 2*VV_kk)+VV_xx,(52*VV_kk)+VV_yy,( 6*VV_kk)+VV_xx,
       (48*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,(50*VV_kk)+VV_yy},
      {( 1*VV_kk)+VV_xx,(28*VV_kk)+VV_yy,
       ( 5*VV_kk)+VV_xx,(30*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,
       (47*VV_kk)+VV_yy,( 1*VV_kk)+VV_xx,(51*VV_kk)+VV_yy,
       ( 1*VV_kk)+VV_xx,(28*VV_kk)+VV_yy,0,0,0,0,
       ( 3*VV_kk)+VV_xx,(38*VV_kk)+VV_yy},
      {( 1*VV_kk)+VV_xx,( 2*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,
       ( 6*VV_kk)+VV_yy,( 5*VV_kk)+VV_xx,(24*VV_kk)+VV_yy,
       ( 1*VV_kk)+VV_xx,(26*VV_kk)+VV_yy,( 1*VV_kk)+VV_xx,
       ( 2*VV_kk)+VV_yy,0,0,0,0,
       ( 3*VV_kk)+VV_xx,(13*VV_kk)+VV_yy},
      {( 6*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,
       (25*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,
       ( 6*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,( 2*VV_kk)+VV_xx,
       (27*VV_kk)+VV_yy,( 6*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,0,0,
       ( 9*VV_kk)+VV_xx,(27*VV_kk)+VV_yy},
      {(15*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,(22*VV_kk)+VV_xx,
       (25*VV_kk)+VV_yy,(26*VV_kk)+VV_xx,(27*VV_kk)+VV_yy,
       (22*VV_kk)+VV_xx,(29*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,
       (29*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,(25*VV_kk)+VV_yy,0,0,
       (20*VV_kk)+VV_xx,(27*VV_kk)+VV_yy},
      {( 7*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,( 8*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,
       (11*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,( 9*VV_kk)+VV_xx,
       (23*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,(13*VV_kk)+VV_yy,
       ( 7*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,( 8*VV_kk)+VV_xx,
       (10*VV_kk)+VV_yy},
      {(12*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,(16*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,
       (13*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,(12*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,( 9*VV_kk)+VV_yy},
      {(18*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,(20*VV_kk)+VV_xx,
       ( 7*VV_kk)+VV_yy,(21*VV_kk)+VV_xx,( 7*VV_kk)+VV_yy,
       (21*VV_kk)+VV_xx,(13*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (23*VV_kk)+VV_yy,(17*VV_kk)+VV_xx,(23*VV_kk)+VV_yy,
       (18*VV_kk)+VV_xx,(10*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (11*VV_kk)+VV_yy},
      {(17*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(19*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(21*VV_kk)+VV_xx,(40*VV_kk)+VV_yy,
       (21*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,(20*VV_kk)+VV_xx,
       (46*VV_kk)+VV_yy,(18*VV_kk)+VV_xx,(43*VV_kk)+VV_yy,
       (17*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(18*VV_kk)+VV_xx,
       (32*VV_kk)+VV_yy},
      {(13*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(15*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(16*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,
       (12*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,(13*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,0,0,0,0,
       (14*VV_kk)+VV_xx,(42*VV_kk)+VV_yy},
      {( 9*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(11*VV_kk)+VV_xx,
       (31*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,(43*VV_kk)+VV_yy,
       ( 8*VV_kk)+VV_xx,(46*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,
       (46*VV_kk)+VV_yy,( 7*VV_kk)+VV_xx,(40*VV_kk)+VV_yy,
       ( 9*VV_kk)+VV_xx,(31*VV_kk)+VV_yy,(10*VV_kk)+VV_xx,
       (32*VV_kk)+VV_yy}};
 /* 14 Segmente mit je max. 7 Punkte und Punkt zum AusfÅllen */
 
 int PV_zeichenfeld[12][14]=       
    {{1,1,1,1,1,1,0,0,0,0,0,0,0,0},    /* 0 */
     {0,1,1,0,0,0,0,0,0,0,0,0,0,0},    /* 1 */
     {1,1,0,1,1,0,1,1,0,0,0,0,0,0},    /* 2 */
     {1,1,1,1,0,0,1,1,0,0,0,0,0,0},    /* 3 */
     {0,1,1,0,0,1,1,1,0,0,0,0,0,0},    /* 4 */
     {1,0,1,1,0,1,1,1,0,0,0,0,0,0},    /* 5 */
     {1,0,1,1,1,1,1,1,0,0,0,0,0,0},    /* 6 */
     {1,1,1,0,0,0,0,0,0,0,0,0,0,0},    /* 7 */
     {1,1,1,1,1,1,1,1,0,0,0,0,0,0},    /* 8 */
     {1,1,1,1,0,1,1,1,0,0,0,0,0,0},    /* 9 */
     {1,0,0,1,1,1,0,0,0,0,0,0,0,0},    /* C */
     {1,1,0,0,0,1,1,1,0,0,0,0,0,0}};   /* ¯ */
     /* Ascii-Zeichen von 0-12 mit je 14 Segmenten */

 /* alle 14 Segmente */
 for (PV_segment=0;PV_segment<=13;PV_segment++)                  
     {
     /* Segment aktiv? */
     if (PV_zeichenfeld[VV_zeichen][PV_segment]==1)
	{
        setcolor(MC_on_colour);
        setfillstyle(SOLID_FILL,MC_on_colour);
        PV_border=MC_on_colour;
	}
     else
	{
        setcolor(MC_off_colour);
        setfillstyle(SOLID_FILL,MC_off_colour);
        PV_border=MC_off_colour;
	}
     /* Segment zeichnen */
     drawpoly(PV_segpunkte[PV_segment],PV_punktfeld[PV_segment]);
     /* Segment ausfÅllen */
     floodfill(PV_punktfeld[PV_segment][14],
               PV_punktfeld[PV_segment][15],PV_border);
     }                                                  
 }


void MP_portausgabe(unsigned int VV_adresse,
                    char VV_bit,char VV_wert)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Portausgabe                                                    */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 unsigned int PV_daten,LV_ii;
 unsigned int PV_zahl=1;
 /* Daten einlesen */
 PV_daten=inportb(VV_adresse);

 /* Bit in Dezimalwert umwandeln */
 for (LV_ii=0;LV_ii<VV_bit;LV_ii++)                                    
     PV_zahl=PV_zahl*2;                                       

 if(VV_wert==0)
  {
    /* 1 Bit auf 0 setzen */
    PV_daten=PV_daten&(255-PV_zahl);
  }
 else
  {
    /* 1 Bit auf 1 setzen */
    PV_daten=PV_daten|PV_zahl;
  };

 /* Daten ausgeben */
 outportb(VV_adresse,PV_daten);
 }


void MP_totzeit()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Totzeiten bestimmen                                            */
/* Bemerkung:                                                      */
/*  Das RC-Glied besitzt negativen Temp.Koeffizient:               */
/*  Kuerzere Zeiten = hoehere Temperatur                           */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 unsigned int PV_ii,PV_altwert,PV_neuwert;

 clrscr();
 MP_datumzeit(60,1,MC_tbeides,MC_tsofort);
 gotoxy(1,1);

 printf("\n\n\n\n   Bestimmen der Totzeiten:");
 printf("\n\n   T_aus =");
 printf("\n\n   T_ein =");
 PV_neuwert=MV_kaldaten[1][MV_anzahl];

 PV_ii=0;
 do
  {
  PV_altwert=PV_neuwert+1;

  /* 500ms warten */
  delay(500);                                           

  PV_ii=PV_ii+5;

  /* Monoflop antriggern */
  MP_portausgabe(MC_portaddr+2,0,0);
  MP_portausgabe(MC_portaddr+2,0,1);
  MP_portausgabe(MC_portaddr+2,0,0);
  /* Temp. messen */
  PV_neuwert=MP_timer();                                      
  MP_datumzeit(60,1,MC_tbeides,0);
  }
 while(PV_neuwert<=PV_altwert);                               
 /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* Warten, solange alter A/D Wert kleiner oder gleich neuem Wert  */
 /* = Temperatur messen, solange neue Temperatur groesser gleich   */
 /*   der zuvor gemessene Temperatur ist                           */
 /* = Warten, bis Temp. anfaengt zu sinken                         */
 /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

 /* T_aus in 100ms Schritten */
 MV_kaldaten[0][0]=PV_ii;
 gotoxy(12,7);
 printf("%3.1fs",(float)PV_ii/10);

 /* Heizung ein */
 MP_portausgabe(MC_portaddr,1,1);
 PV_ii=0;

 do
  {
  PV_altwert=PV_neuwert-1;
  /* 500ms warten */
  delay(500);                                           
  PV_ii=PV_ii+5;
  /* Monoflop antriggern */
  MP_portausgabe(MC_portaddr+2,0,0);
  MP_portausgabe(MC_portaddr+2,0,1);
  MP_portausgabe(MC_portaddr+2,0,0);
  /* Temp. messen */
  PV_neuwert=MP_timer();                                      
  MP_datumzeit(60,1,MC_tbeides,0);
  }
 while(PV_neuwert>=PV_altwert);
 /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 /* Warten, solange alter A/D Wert groesser oder gleich neuem Wert */
 /* = Temperatur messen, solange neue Temperatur niedriger oder    */
 /*   gleich der zuvor gemessenen Temperatur ist                   */
 /* = Warten, bis Temp. anfaengt anzusteigen                       */
 /*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

 /* Heizung aus */
 MP_portausgabe(MC_portaddr,1,0);

 /* T_ein in 100ms Schritten */
 MV_kaldaten[1][0]=PV_ii;
 gotoxy(12,9);
 printf("%3.1fs",(float)PV_ii/10);
 }

void MP_graph_zeichnen(unsigned int VV_xmin,unsigned int VV_dx,
                       unsigned int VV_ymin,unsigned int VV_dy,
                       char *IV_xtext1,char *IV_xtext2,
                       char *IV_ytext1,char *IV_ytext2)
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Graph ausgeben                                                 */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char           LV_ii;
 unsigned int PV_xbegin,PV_ybegin;
 unsigned int PV_laenge,PV_hoehe;
 char           PV_buf[50];
 setcolor(WHITE);
 /* X-Achse */
 line(59,450,580,450);                             
 /* Pfeil X-Achse */
 line(580,450,574,443);                            
 line(580,450,574,457);
 /* Y-Achse */
 line(59,450,59,10);                               
 /* Pfeil Y-Achse */
 line(59,10,52,17);                                
 line(59,10,66,17);

 
 if(VV_xmin==0)
  {
   /* Unterbrechungszeichen X */
   PV_xbegin=0;
  }
 else                                              
  {
  /* wenn X nicht bei 0 beginnt */
  PV_xbegin=1;
  line(92,443,78,457);
  line(95,443,81,457);
  }

 if(VV_ymin==0)
  {
    /* Unterbrechungszeichen Y */
    PV_ybegin=0;
  }
 else
  {
  /* wenn Y nicht bei 0 beginnt */
  PV_ybegin=1;
  line(52,432,66,418);
  line(52,435,66,421);
  }

  /* Hauptintervall X */
 for(LV_ii=0;LV_ii<=8;LV_ii++)
    line(119+LV_ii*11*5,443,119+LV_ii*11*5,457);

 /* Hauptintervall Y */
 for(LV_ii=0;LV_ii<=8;LV_ii++)
    line(52,405-LV_ii*9*5,66,405-LV_ii*9*5);

 /* Nebenintervall X */
 for(LV_ii=5*PV_xbegin;LV_ii<=43;LV_ii++)
    line(75+LV_ii*11,447,75+LV_ii*11,453);

 /* Nebenintervall Y */
 for(LV_ii=5*PV_ybegin;LV_ii<=43;LV_ii++)
    line(56,441-LV_ii*9,62,441-LV_ii*9);
 outtextxy(35,465,"0");

 /* Achsbeschriftung X */
 for(LV_ii=1;LV_ii<=9;LV_ii++)
    {
      if(VV_xmin==0)
       sprintf(PV_buf,"%3lu",VV_xmin+(long)LV_ii*VV_dx);
      else
       sprintf(PV_buf,"%3lu",VV_xmin+(long)(LV_ii-1)*VV_dx);

      outtextxy(46+LV_ii*11*5,465,PV_buf);
    }

 PV_laenge=textwidth(IV_xtext1);

 if(textwidth(IV_xtext2) > PV_laenge)
   PV_laenge=textwidth(IV_xtext2);

 PV_hoehe=textheight(IV_xtext1);
 outtextxy((585+PV_laenge/2)-textwidth(IV_xtext1)/2,448-PV_hoehe,IV_xtext1);

 /* falls String nicht NULL */
 if (strcmp(IV_xtext2,"")!=0)                          
    {
    line(585,450,585+PV_laenge,450);
    outtextxy((585+PV_laenge/2)-textwidth(IV_xtext2)/2,454,IV_xtext2);
    }

 /* Achsbeschriftung Y */
 for(LV_ii=1;LV_ii<=9;LV_ii++)
    {
       if(VV_ymin==0)
         sprintf(PV_buf,"%5lu",VV_ymin+(long)LV_ii*VV_dy);
       else
         sprintf(PV_buf,"%5lu",VV_ymin+(long)(LV_ii-1)*VV_dy);

       outtextxy(0,447-LV_ii*9*5,PV_buf);
    }

 PV_laenge=textwidth(IV_ytext1);

 if(textwidth(IV_ytext2) > PV_laenge)
   PV_laenge=textwidth(IV_ytext2);

 PV_hoehe=textheight(IV_ytext1);
 outtextxy(50-PV_laenge/2-textwidth(IV_ytext1)/2,0,IV_ytext1);

 if (strcmp(IV_ytext2,"")!=0)                         
    {
    /* falls String nicht NULL */
    line(50-PV_laenge,2+PV_hoehe,50,2+PV_hoehe);
    outtextxy(50-PV_laenge/2-textwidth(IV_ytext2)/2,6+PV_hoehe,IV_ytext2);
    }
 }


void MP_stat_regler()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Regler mit statischen Grenzen                                  */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char           PV_key,PV_fehler;
 int            PV_zeit,PV_sollwert;
 int            PV_ii,PV_imax,PV_jj;
 unsigned int   PV_zaehler_sollwert;
 unsigned int   PV_zaehler;
 unsigned int   PV_dx,PV_dy;
 unsigned long  PV_dt;
 double         PV_ypunkt;
 float          PV_temp;
 clrscr();
 printf("\n\n\n\n      REGELUNG MIT STATISCHER GRENZE");
 printf("\n      ==============================");
 printf("\n\n\n   In diesem einfachen Fall der Regelung sind die Ein- und");
 printf("\n   Ausschaltschwelle fÅr die Heizung fest durch den Sollwert");
 printf("\n   vorgegeben. Wenn der Istwert kleiner als der Sollwert ist,");
 printf("\n   wird die Heizung eingeschaltet, bei Werten grî·er als der");
 printf("\n   Sollwert wird sie ausgeschaltet.");
 printf("\n\n\n   Geben Sie bitte den Sollwert in ¯C und die Dauer der");
 printf("\n   Regelung in Sekunden ein.");
 printf("\n\n   Sollwert  :");
 printf("\n\n   Regeldauer:");

 MP_datumzeit(60,1,MC_tbeides,MC_tsofort);
 gotoxy(16,19);
 scanf("%d",&PV_sollwert);
 MP_datumzeit(60,1,MC_tbeides,0);
 gotoxy(16,21);
 scanf("%d",&PV_zeit);

 /* Sollwert innerhalb der Kalibrierungsdaten ? */
 if(
    (MV_kaldaten[0][1]<PV_sollwert) &&
    (MV_kaldaten[0][MV_anzahl]>PV_sollwert)
    )
   {
   PV_ii=1;

   /*++++++++++++++++++++++++++++++++++++++++++++++++*/
   /* Berechnung, welcher Kalibrierungswert groesser */
   /* oder gleich dem Sollwert ist                   */
   /*++++++++++++++++++++++++++++++++++++++++++++++++*/
   while(MV_kaldaten[0][PV_ii]<PV_sollwert)
     { PV_ii++; };

   /* Berechnung des ZÑhlerwertes der Solltemperatur */
   PV_zaehler_sollwert=(float)MV_kaldaten[1][PV_ii-1]-
		       (MV_kaldaten[1][PV_ii-1]-MV_kaldaten[1][PV_ii])*
                       (PV_sollwert-MV_kaldaten[0][PV_ii-1])/
                       (MV_kaldaten[0][PV_ii]-MV_kaldaten[0][PV_ii-1]);

   /* Graphik einschalten */
   PV_fehler=MP_graphik_ein();
   if (PV_fehler==MC_false)
      {

        /* Wert zwischen 2 Hauptintervallen */
        PV_dx=PV_zeit/9/5*5+5;

        /* Wert zwischen 2 Hauptintervallen */
        PV_dy=(3/2*PV_sollwert)/9/5*5+5;

        /* Pixel pro Temp.-Intervall */
        PV_ypunkt=5*9/(double)PV_dy;
        MP_graph_zeichnen(0,PV_dx,0,PV_dy,"t","s","T","¯C");

        /* Anzahl der Pixel, die zur VerfÅgung stehen */
        PV_imax=(float)PV_zeit/PV_dx*5*11;

        PV_ii=PV_imax;
        /* Me·intervall in ms */
        PV_dt=(long)PV_zeit*1000/PV_ii;
        /* Sollwert einzeichnen */
        setlinestyle(DOTTED_LINE,0,1);
        line(59,450-PV_sollwert*PV_ypunkt,580,450-PV_sollwert*PV_ypunkt);
        setlinestyle(SOLID_LINE,0,1);

        do
         {
           /* Monoflop antriggern */
           MP_portausgabe(MC_portaddr+2,0,0);
           MP_portausgabe(MC_portaddr+2,0,1);
           MP_portausgabe(MC_portaddr+2,0,0);
           /* Temperatur messen */
           PV_zaehler=MP_timer();
           /* Temp. zu gro·? */
           if(PV_zaehler<PV_zaehler_sollwert)
            {
              /* dann Heizung aus */
              MP_portausgabe(MC_portaddr,1,0);
            }
           else
            {
              /*  sonst Heizung ein */
              MP_portausgabe(MC_portaddr,1,1);
            };

           /* Temp. aus ZÑhlerwert berechnen */

           /* Temp. kleiner als kleinster Kal-Wert? */
           if (PV_zaehler>MV_kaldaten[1][1])
            {
              PV_temp=MV_kaldaten[0][1]-
                      (float)(PV_zaehler-MV_kaldaten[1][1])*
                      (MV_kaldaten[0][2]-MV_kaldaten[0][1])/
                      (MV_kaldaten[1][1]-MV_kaldaten[1][2]);
            }

           /* Temp. grî·er als grî·ter Kal-Wert? */
           if (PV_zaehler<MV_kaldaten[1][MV_anzahl])
            {
              PV_temp=(float)(MV_kaldaten[1][MV_anzahl]-PV_zaehler)*
                  (MV_kaldaten[0][MV_anzahl] - MV_kaldaten[0][MV_anzahl-1])/
                  (MV_kaldaten[1][MV_anzahl-1] -
                  MV_kaldaten[1][MV_anzahl])+MV_kaldaten[0][MV_anzahl];
            }

           /* Temp. im Bereich der Kal-Werte? */
           if ((PV_zaehler<MV_kaldaten[1][1]) &&
               (PV_zaehler>MV_kaldaten[1][MV_anzahl]))
            {
              PV_jj=1;
              while(MV_kaldaten[1][PV_jj]>PV_zaehler)
               {
                 PV_jj++;
               };
              PV_temp=(float)(MV_kaldaten[1][PV_jj-1]-PV_zaehler)*
                      (MV_kaldaten[0][PV_jj]-MV_kaldaten[0][PV_jj-1])/
                      (MV_kaldaten[1][PV_jj-1]-MV_kaldaten[1][PV_jj])+
                      MV_kaldaten[0][PV_jj-1];
            };

           /* Tastatur abfragen */
          if(kbhit())
           {
             PV_key=getch();
           };

          /* Zuerst Cursor setzen, dann Punkte zeichnen*/
          if(PV_ii==PV_imax)
           {
             moveto(59,450-PV_temp*PV_ypunkt);
           }
          else
           {
             lineto(59+PV_imax-PV_ii,450-PV_temp*PV_ypunkt);
           };
        
          /* Datum und Zeit anzeigen */
          MP_datumzeit(60,1,MC_tbeides,0);

          /* Berechnetes Zeitintervall */
          delay(PV_dt);

          /* warten */
          PV_ii--;
         }
      while(PV_key!=MC_kbesc && PV_ii>0);
      /* Bis alles gezeichnet oder ESC*/

      /* Heizung aus */
      MP_portausgabe(MC_portaddr,1,0);
      getch();
      MP_graphik_aus();
      }
   }
 else
   {
   clrscr();
   printf("\n\n\n   Sollwert liegt nicht innerhalb der aufgenommenen");
   printf("\n   Kalibrierkurve.");
   printf("\n\n\n   Weiter mit Taste...");
   fflush(stdin);
   getch();
   }
 }


void MP_dyn_regler()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Regler mit dynamischen Grenzen                                 */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 int PV_sollwert,ugrenze,ogrenze;
 clrscr();
 printf("\n\n\n\n   Leider aus ZeitgrÅnden nicht implementiert :-(.");
 getch();
 }


void main()
/*******************************************************************/
/* Aufgabe:                                                        */
/*  Main = Hauptprogramm                                           */
/* Parameter: -                                                    */
/* Rueckgabewert: -                                                */
/*******************************************************************/
 {
 char PV_key;

 MV_anzahl=0;

 do
  {
  clrscr();
  printf("\n\n\n\n      2pregler - ZWEIPUNKTREGLER");
  printf("\n      ==============================");
  printf("\n      Copyright 2000-2015 Rolf Hemmerling");
  printf("\n      Licensed under the Apache License, Version 2.0");
  printf("\n\n\n   1) Eichkurve aufnehmen");
  printf("\n   2) Eichkurve laden");
  printf("\n   3) Eichkurve anzeigen");
  printf("\n   4) Temperatur regeln");
  printf("\n ESC) Programm beenden");
  printf("\n\n Bitte waehlen Sie: ");
  MP_datumzeit(60,1,MC_tbeides,MC_tsofort);
  do
   {
   fflush(stdin);

   /* auf Tastendruck warten */
   do                                   
    {
    MP_datumzeit(60,1,MC_tbeides,0);
    }
   while(!kbhit());

   PV_key=getch();
   }
  while( PV_key!=MC_kbesc && PV_key!='1' && PV_key!='2' &&
         PV_key!='3' && PV_key!='4');

  if(PV_key=='1') MP_eichung();
  if(PV_key=='2') MP_kurve_laden();
  if(PV_key=='3') MP_anzeigen();
  if(PV_key=='4') MP_regler();
  }
 while(PV_key!=MC_kbesc);

 clrscr();

 }

/* EOF */

