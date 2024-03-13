/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

// librerias y defines 
#include "BufferedSerial.h"
#include "BusOut.h"
#include "Thread.h"
#include "cmsis_os2.h"
#include "mbed.h"



// prototipos 
void motor_paso_der(void);
void motor_paso_izq(void);
void recibir_datos(void);

//tareas
Thread hilo_motor_paso_der(osPriorityNormal,512);
Thread hilo_motor_paso_izq(osPriorityNormal,512);
Thread hilo_recibir_datos(osPriorityNormal,512);

Mutex p_serie;

// pines 
BusOut bobinas(D3,D4,D5,D6);
BufferedSerial pc(USBTX,USBRX);



// variables globales 
const char pasos[4] = {0xc,0x6,0x3,0x9}; // secuencia del motor paso
int i=0;  // puntero del vector
char men[20];
bool der=false;
bool izq=false;

int main()
{
    hilo_motor_paso_der.start(motor_paso_der);
    hilo_motor_paso_izq.start(motor_paso_izq);
    hilo_recibir_datos.start(recibir_datos);

    sprintf(men, "Arranque... \n\r");
    pc.write(men, sizeof(men));
    while (true) {
    }
}

void motor_paso_der(void)
{
    while(true)
    {
        if (der==true)
        {
            p_serie.lock();
            i++;
            if (i==4) i=0;
            bobinas = pasos[i];
            sprintf(men,"Giro derecha %u \n\r", i);
            pc.write(men, sizeof(men));
            p_serie.unlock();
            ThisThread::sleep_for(250ms);
        }
    }

}


void motor_paso_izq(void)
{
    while(true)
    {
        if (izq==true)
        {
            p_serie.lock();
            i--;
            if (i==-1) i=3;
            bobinas = pasos[i];
            sprintf(men,"Giro izquierda %u \n\r", i);
            pc.write(men, sizeof(men));
            p_serie.unlock();
            ThisThread::sleep_for(250ms);
        }
    }

}


void recibir_datos (void)
{
    char c[1];
    
    while(true)
    {
      if (pc.readable())
      {
        pc.read(c,1);
        /*
        if          (c[0] =='a'){der=true; izq=false;}
        else if     (c[0] =='b'){der=false; izq=true;}
        else if     (c[0] =='c'){der=false; izq=false;}
        else                    {der=false; izq=false;}*/

        
        switch (c[0]) 
        {
            case 'a': der=true; izq=false; break;
            case 'b': der=false; izq=true; break;
            case 'c': der= false; izq= false; break;
            //default:  der= false; izq= false; break;  
        }
      }
    }
}