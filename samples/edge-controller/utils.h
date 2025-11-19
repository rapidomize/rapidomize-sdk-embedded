#ifndef RPZ_UTILS_H_
#define RPZ_UTILS_H_

#include <Arduino.h>

#define LED 2       //on board LED if any!
#define BUZZER 12


class Utils{
    public: 
        static void indicate(){
            digitalWrite(BUZZER,HIGH);
            delay(500);
            digitalWrite(BUZZER,LOW);
        }

        static void buzzer(int count){
            for(int i=0; i< count; i++){
                digitalWrite(BUZZER, HIGH);
                delay(300); 
                digitalWrite(BUZZER, LOW) ;
                delay(300); 
            }
        }
};

#endif /* RPZ_UTILS_H_ */


