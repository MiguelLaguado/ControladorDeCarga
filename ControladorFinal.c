//Programa utilizado: PicC
//Pic 16f877
#include <16f877.h>
#device adc=10
#fuses HS,NOWDT
#use delay(clock = 20MHz)
#use standard_io(B)
//Valores Predeterminados del controlador

//Funciones del controlador
void inicio(void);
float SensorPanel();
float SensorBat();
float SensorCor();
float SensorVol1();
//Leer los valores de los sensores 
void bateria();
void cargapwm();
void sencor();
long i;
float vpanel,voc,vbat,cpanel,volt1;
float nominal=18;
// Deltat es el tiempo de duración del código, se realizó su medición usando el software proteus 8.
float deltat=0.309523;  //DELTAT DEFINITIVO
float flota=14.7;  //100%  
float low=12;   //SC=0%
float acor;
float sc;
float q;
float qn;
float qpor;

void inicio(){
port_b_pullups(TRUE);
setup_timer_2(t2_div_by_1,249,1); //20Khz
setup_adc_ports(AN0_AN1_AN2_AN3_AN4_AN5);
setup_adc(ADC_CLOCK_INTERNAL);
setup_ccp1(ccp_pwm);
setup_ccp2(ccp_pwm);
set_pwm1_duty(0);
set_pwm2_duty(0);
output_high(PIN_B7);
output_low(PIN_B6);
   }
   
   
void sencor(){

   if (vpanel<14){
 output_low(PIN_B1);
  set_pwm1_duty(0);
  }
  else if (vpanel>14){
  output_high(PIN_B1);
  }
 
 acor=0;
  for(i=0; i<=100;i++)
   {
  volt1=SensorVol1();
   volt1=volt1*(2.5/5);
   volt1=(volt1*5)/1023;
   cpanel=SensorCor();   
   cpanel=(cpanel*5)/1023;
   cpanel= (cpanel-volt1)/0.108;
   acor= acor + cpanel;
  }
   acor=acor/100;
  acor=acor+0.17;
}
   
void bateria(){
q=acor*deltat;
qn=q/3600;
qpor= (qn/nominal)*100;
sc=sc + qpor;
}

void main(){
     
   inicio();
 
    voc=SensorBat();
    voc=(voc*5)/1023;
    sc= (100*voc) - 1200;
    
  set_pwm1_duty(910);
  while(true){
   
   vbat=SensorBat();
   vbat=(vbat*5)/1023;
    vpanel=SensorPanel();
   vpanel=(vpanel*5)/1023;
  //Formulas del anteproyecto (Referencias)
  //Estimación inicial usando el voltaje de circuito abierto
  
   
sencor();
bateria();
cargapwm();
output_high(PIN_B6);
  } 
}
//Función de la estimación del estado de carga de la bateria

void cargapwm(){
if (sc>0 && vbat>low){   
   set_pwm2_duty(920);

  if (sc<=60 && vbat<flota){
   set_pwm1_duty(920);
  output_high(PIN_B4);
  output_low(PIN_B3);
  output_low(PIN_B2);
 output_low(PIN_B5);
  
  }
  else if((sc>60)&&(sc<=96) && vbat<flota){
 output_high(PIN_B3);
 output_low(PIN_B2);
 output_low(PIN_B4);
 output_low(PIN_B5);
         if(sc>=60 && sc<65){
         set_pwm1_duty(818);}      //80%
         else if(sc>=65 && sc<70){
         set_pwm1_duty(716);}       //70% 
         else if(sc>=70 && sc<75){
         set_pwm1_duty(613);}       //60%  
         else if(sc>=75 && vbat<80){
         set_pwm1_duty(511);}       //50%   
         else if(sc>=80 && sc<85){
         set_pwm1_duty(409);}       //40% 
         else if(sc>=85 && sc<90){
         set_pwm1_duty(306);}       //30%
         else if(sc>=90 && sc<93){
         set_pwm1_duty(204);}       //20%
         else if(sc>=93 && sc<96){
         set_pwm1_duty(102);}       //10%
  }
 else if((sc>96)||(vbat>flota)){   

 set_pwm1_duty(0);
 
    if (sc>96){
    
     output_low(PIN_B3);
  output_low(PIN_B4);
  output_high(PIN_B2);
  output_low(PIN_B5);
    }
   else{
 
      vbat=SensorBat();
   vbat=(vbat*5)/1023;
   if (vbat<13.7){
      set_pwm1_duty(910);
  }
   else if (vbat>13.7){
   set_pwm1_duty(0);
   output_low(PIN_B3);
  output_low(PIN_B4);
  output_high(PIN_B2);
  output_low(PIN_B5);
   }
   }
 
 }
}
else if ((sc<0)||(vbat<low)){

 output_low(PIN_B3);
 output_low(PIN_B2);
 output_low(PIN_B4);
 output_high(PIN_B5);


if (vpanel<14){
 set_pwm1_duty(910);
 set_pwm2_duty(0);
}
else
{
 set_pwm1_duty(910);
 set_pwm2_duty(910);
}

}
 
}
 
float SensorPanel (){
   long span;
   float panel;
   //delay_ms(500);
   set_adc_channel(1);  //Se uso el canal AN0 del PIC
   delay_ms(1);
   span=read_adc();
   panel=span*(120/20) ;  //Acondicionamiento del divisor de voltaje con Resistencias de 100k y 20k
   return panel;
}

float SensorBat (){
   long sbat;
   float bate;
   //delay_ms(500);
   set_adc_channel(0);  //Se uso el canal AN1 del PIC
   delay_ms(1);
   sbat=read_adc();
   bate=sbat*(120/20) ;  //Acondicionamiento del divisor de voltaje con resistencias de 100k y 20k
   return bate;
}

float SensorVol1 (){
   float svol1;
   //float vol1;
   //delay_ms(500);
   set_adc_channel(2);  //Se uso el canal AN2 del PIC
   delay_ms(1);
   svol1=read_adc();
   //vol1=svol1*(2.5/5);  
   return svol1;
}



float SensorCor (){
   float scor;
   set_adc_channel(3);
   delay_ms(1); //Se uso el canal AN3 del PIC
   scor=read_adc();
   return scor;
}

//Desarrollado por: Miguel Laguado y Eduardo Luna, Universidad Francisco de Paula Santander

