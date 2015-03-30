#ifndef ZWEIPREGLER_H
#define ZWEIPREGLER_H

/**
 *  @package   2pregler
 *  @file      2pregler.h
 *  @brief     Include-Datei fuer das Programm 
 *             "Zweipunktregler fuer eine Heizung"   
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
 *  2pregler.cpp - Include-Datei fuer das Programm 
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
/* Debug-Schalter                                                  */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @def      DEBUG
 *  @brief    DEBUG
 */
#define DEBUG 1

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Include-Dateien                                                 */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include<dos.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#include<conio.h>
#include<graphics.h>

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Konstanten-Definitionen                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @def      MC_tsofort
 *  @brief    Sofort
 */
#define MC_tsofort  1
/**
 *  @def      MC_tsekunde
 *  @brief    Sekunde
 */
#define MC_tsekunde 0
/**
 *  @def      MC_tdatum
 *  @brief    Datum
 */
#define MC_tdatum   1
/**
 *  @def      MC_tzeit
 *  @brief    Zeit
 */
#define MC_tzeit    2
/**
 *  @def      MC_tbeides
 *  @brief    Beides
 */
#define MC_tbeides  3

#define MC_true       1
#define MC_false      0

/* Tastaturcodes */

/**
 *  @def      MC_kbleft
 *  @brief    Tastatur-Code links
 */
#define MC_kbleft     0x004B
/**
 *  @def      MC_kbright
 *  @brief    Tastatur-Code rechts
 */
#define MC_kbright    0x004D
/**
 *  @def      MC_kbesc
 *  @brief    Tastatur-Code ESC
 */
#define MC_kbesc      0x001B
/**
 *  @def      MC_kbenter
 *  @brief    Tastatur-Code ENTER / RETURN
 */
#define MC_kbenter    0x000D

/**
 *  @def      MC_treiberpfad
 *  @brief    Treiberpfad fuer Graphik
 *            *define MC_treiberpfad "c:\\borlandc\\bgi"
 */
#define MC_treiberpfad "c:\\borlandc\\bgi"

/**
 *  @def      MC_portaddr
 *  @brief    Portadresse der Parallelen Schnittstelle im IBM-AT
 */
#define MC_portaddr 0x0278

/**
 *  @def      MC_on
 *  @brief    Logischer Zustand "An"
 */
#define MC_on          1
/**
 *  @def      MC_off
 *  @brief    Logischer Zustand "Aus"
 */
#define MC_off         0

/* Farben */

/**
 *  @def      MC_on_colour
 *  @brief    MC_on_colour
 */
#define MC_on_colour    GREEN
/**
 *  @def      MC_off_colour
 *  @brief    MC_off_colour
 */
#define MC_off_colour   BLACK

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* Funktions-Definitionen ( Prototypen )                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/**
 *  @fn       MP_cursor_entfernen
 *  @brief    Der Cursor wird in einen nicht sichtbaren Bereich gesetzt  
 */
void MP_cursor_entfernen();

/**
 *  @fn       MP_datumzeit(char VV_xx, char VV_yy, char VV_art, char VV_wann)
 *  @param    VV_xx Position x
 *  @param    VV_yy Position y
 *  @param    VV_art 
 *            1 = nur Datum anzeigen
 *            2 = nur Zeit anzeigen
 *            3 = beides anzeigen
 *  @param    VV_wann MC_tsofort neu anzeigen, auch wenn noch keine
 *                    Sekunde vergangen ist
 *  @brief    Datum,Zeit anzeigen   
 */
void MP_datumzeit(char VV_xx, char VV_yy, char VV_art, char VV_wann);

/**
 *  @fn       MP_speichern
 *  @return   FILE* Dateizeiger
 *  @brief    Datei speichern 
 */
FILE * MP_speichern(void);

/**
 *  @fn       MP_laden
 *  @return   FILE* Dateizeiger
 *  @brief    Datei laden
 */
FILE * MP_laden(void);

/**
 *  @fn       MP_eichung
 *  @brief    Eichkurve aufnehmen
 */
void MP_eichung(void);

/**
 *  @fn       MP_kurve_laden
 *  @brief    Eichkurve laden
 */
void MP_kurve_laden(void);

/**
 *  @fn       MP_anzeigen
 *  @brief    Eichkurve anzeigen
 */
void MP_anzeigen(void);

/**
 *  @fn       MP_regler
 *  @brief    Regelermenue
 */
void MP_regler(void);

/**
 *  @fn       MP_timer
 *  @return   int Zaehlerwert
 *  @brief    Timer
 */
unsigned int MP_timer(void);

/**
 *  @fn       MP_graphik_ein
 *  @return   char 8-bit Fehlercode
 *  @brief    Grafik einschalten
 */
char MP_graphik_ein(void);

/**
 *  @fn       MP_graphik_aus
 *  @brief    Grafik ausschalten
 */
void MP_graphik_aus(void);

/**
 *  @fn       MP_led_anzeige(unsigned int VV_zeichen, unsigned int VV_xx,
 *                           unsigned int VV_yy, float VV_kk)
 *  @param    VV_zeichen
 *  @param    VV_xx Position x
 *  @param    VV_yy Position y
 *  @param    VV_kk
 *  @brief    LED Anzeige - 14-Segment-Anzeige definieren   
 */
void MP_led_anzeige(unsigned int VV_zeichen, unsigned int VV_xx,
                    unsigned int VV_yy, float VV_kk);

/**
 *  @fn       MP_portausgabe(unsigned int VV_adresse,char VV_bit,
 *                           char VV_wert)
 *  @param    VV_adresse
 *  @param    VV_bit
 *  @param    VV_wert
 *  @brief    Portausgabe   
 */
void MP_portausgabe(unsigned int VV_adresse,char VV_bit,
                    char VV_wert);

/**
 *  @fn       MP_totzeit
 *  @brief    Totzeiten bestimmen
 *            Das RC-Glied besitzt negativen Temp.Koeffizient:
 *            Kuerzere Zeiten = hoehere Temperatur
 */
void MP_totzeit(void);

/**
 *  @fn       MP_graph_zeichnen(unsigned int VV_xmin,unsigned int VV_dx,
 *                              unsigned int VV_ymin,unsigned int VV_dy,
 *                              char *IV_xtext1,char *IV_xtext2,
 *                              char *IV_ytext1,char *IV_ytext2)
 *  @param    VV_xmin
 *  @param    VV_dx
 *  @param    VV_ymin

 *  @param    VV_dy
 *  @param    IV_xtext1
 *  @param    IV_xtext2
 *  @param    IV_ytext1
 *  @param    IV_ytext2
 *  @brief    Graph ausgeben  
 */
void MP_graph_zeichnen(unsigned int VV_xmin,unsigned int VV_dx,
                       unsigned int VV_ymin,unsigned int VV_dy,
                              char *IV_xtext1,char *IV_xtext2,
                              char *IV_ytext1,char *IV_ytext2);

/**
 *  @fn       MP_stat_regler
 *  @brief    Regler mit statischen Grenzen
 */
void MP_stat_regler(void);

/**
 *  @fn       MP_dyn_regler
 *  @brief    Regler mit dynamischen Grenzen
 */
void MP_dyn_regler(void);

/**
 *  @fn       main
 *  @brief    Hauptprogramm
 */
void main(void);

#endif


