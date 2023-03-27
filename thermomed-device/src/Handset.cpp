
#include <cmath>

#include "main.h"
#include "Handset.h"
#include "TreatmentTimeHandler.h"
#include "PushButton.h"
#include "MLX90614.h"
#include "Wireone.h"




double avg_val_obj[numReadings]; // array for object avg calculation

volatile double TestTemp;
double val_obj; // object temperature

double a = 0.7114;
double b = 7.793;
        

double p1 = -3.3333*pow(10,-5);
double p2 = 0.00369;
double p3 = 0.1648;
double p4 = -6.576;
double total_obj = 0;



HandsetClass::HandsetClass(){
    //
}


void HandsetClass::init(){
    

    TempSensor.begin(MLX90614_I2CADDR, &wireOne);

}


double HandsetClass::readObjTemp(){

    if(avg_val_obj[0] == 0){
        for(int i =0;i<numReadings;i++){
            total_obj = total_obj - avg_val_obj[i]; // delete foremost data entry in array
            avg_val_obj[i] = TempSensor.readObjectTempC(); // read the input object
            total_obj = total_obj + avg_val_obj[i]; // append to data array
        }
    }else{
        for(int i =0;i<numReadings-1;i++){
            avg_val_obj[i] = avg_val_obj[i+1];
        }
        avg_val_obj[numReadings-1] = TempSensor.readObjectTempC();
        total_obj = 0;
        for(int i =0;i<numReadings;i++){
            total_obj = total_obj + avg_val_obj[i];
        }
    }
    
    val_obj = total_obj / numReadings;


//fitting
    double inverse = (val_obj-9.071)/0.6043;
    double inverse_2= (inverse + 1.079)/1.017;
return inverse_2;

//TempSensor.readObjectTempC();



/*
    val_obj = 0;

    for(int i =0;i<numReadings;i++){
        val_obj += TempSensor.readObjectTempC();
    }            
    val_obj = val_obj / numReadings;
*/
        


    //lens absorpation fitting
    //inverse = (val_obj-b)/a;
    //val_obj = val_obj + (p1*pow(inverse,3) + p2*pow(inverse,2) + p3*inverse + p4);

    //return val_obj;
    //return TempSensor.readObjectTempC();
}

double HandsetClass::readAmbTemp(){
    return TempSensor.readAmbientTempC();
}

void HandsetClass::loop(){

    // Update Object temperature
    Temp = readObjTemp();
    TestTemp = Temp;
    settings.temperatureActual = Temp;
    rtsys.set_temp_pv(Temp);

    // Check for start Button
    startButton.checkPin();

}

void HandsetClass::handle(){

    //uint32_t now = millis();

    startButton.checkPin();

    /* count 1 sec. after boot 
    if (now > 1000) {

         count up only if there is no recent packet received 
        if (now > miliPktTemperature + miliPktTemperatureThreshold) {
            settings.faultCount++;
        }
    }
    */
}
