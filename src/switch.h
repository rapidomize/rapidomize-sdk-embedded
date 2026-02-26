#ifndef RPZ_SWITCH_H_
#define RPZ_SWITCH_H_

#include <FunctionalInterrupt.h>
/*Input triggers the Buzzer code*/
#include "peripheral.h" 

namespace rpz{

const char *Switch_tmpl = R"(
<div  class="card">
    <div class="row pos-r"><h3>%s</h3><input type="checkbox" name="%s" %s  class="pos-a" style="right: 0px;"></div>
    <table>
        <tr><td>GPIO</td><td>
        <input type="number" name="%s_GPIO" value="%d" disabled>
        <!--<select name="_s_GPIO">
            <option value="14">14</option>
            <option value="13">13</option>
        </select>--></td>
    </table>  
</div>  
)";  

const char *SW_MSG  = R"({"%s":%d})";

//https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/  
const int IN1=36; //GPIO36
const int IN2=39; //GPIO39

volatile bool triggered = false;
// For debouncing
unsigned long lsttime = 0;
const unsigned long  DEBOUNCE_DELAY = 500L;  // in milliseconds  

//N.B. debounce in the isr in a memeber funtion err - dangerous relocation: l32r: literal placed after use  
void IRAM_ATTR handleInterrupt() {
  //Serial.println("isr1");
  unsigned long now = millis();
  if (now - lsttime > DEBOUNCE_DELAY) {
    triggered = true;
    lsttime = now;
    Serial.println("isr2");
  }
}

class Switch: public Peripheral{
    uint8_t gpio;

    public: 
    Switch(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
      sprintf(name, "DIN_%d", seq);

      String pname = name; pname+="_GPIO";

      //default
      conf[pname] = seq == 1? 14 : 13;
      configure();
      gpio = (uint8_t)conf[pname];
    }

    void init(JsonDocument *jconf) {
      Peripheral::init(jconf);
      if(!enabled) return;

      isr = true;
      pinMode(gpio, INPUT_PULLUP);
      // attachInterrupt(digitalPinToInterrupt(gpio), std::bind(&Switch::handleInterrupt, this), FALLING);
      attachInterrupt(digitalPinToInterrupt(gpio), handleInterrupt, CHANGE);//LOW, CHANGE, RISING, FALLING
      // Parse configuration if provided
      // Format: "slave_addr,de_pin,rx_pin,tx_pin,baud_rate" or use defaults
      Serial.printf(PSTR("%s initialized with gpio: %d\n"), name, gpio);
    }

    char * confpg(){
        char *fr = (char *) malloc(4096);
        sprintf(fr, Switch_tmpl, name, name, enabled?"checked":"", name, gpio);
        return fr;
    }
    
    char * read(){
      if(!enabled) return nullptr;

      if(triggered) {
          //toggle for next event
          triggered = false;
          sprintf(data, SW_MSG, name, 1);
          return data;
      }
      return nullptr;
		}
};


}

#endif