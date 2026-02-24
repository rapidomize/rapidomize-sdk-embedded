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
        <input type="number" name="GPIO" value="%d" disabled>
        <!--<select name="GPIO" value="36" disabled>
            <option value="36">36</option>
            <option value="39">39</option>
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
const unsigned long  DEBOUNCE_DELAY = 1550L;  // in milliseconds  

//N.B. debounce in the isr in a memeber funtion err - dangerous relocation: l32r: literal placed after use  
void IRAM_ATTR handleInterrupt() {
    unsigned long now = millis();
    if (now - lsttime > DEBOUNCE_DELAY) {
      triggered = true;
      lsttime = now;
      Serial.println("isr");
    }
}

class Switch: public Peripheral{
    const char * dconf = "{\"GPIO\":12}";

    public: 
    Switch(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
      name = "DIN_";
      name+=seq;
      configure(dconf);

      conf["GPIO"] = seq == 1? 12 : 13;
    }

    void init(JsonDocument *jconf) {
      Peripheral::init(jconf);
      if(!enabled) return;

      isr = true;
      pinMode((int)conf["GPIO"], INPUT_PULLUP);
      // attachInterrupt(digitalPinToInterrupt((int)conf["GPIO"]), std::bind(&Switch::handleInterrupt, this), FALLING);
      attachInterrupt(digitalPinToInterrupt((int)conf["GPIO"]), handleInterrupt, CHANGE);//LOW, CHANGE, RISING, FALLING
    }

    char * confpg(){
        char *fr = (char *) malloc(4096);
        const char * pname = name.c_str();
        sprintf(fr, Switch_tmpl, pname, pname, enabled?"checked":"", (int)conf["GPIO"]);
        return fr;
    }
    
    char * read(){
      if(!enabled) return nullptr;

      if(triggered) {
          //toggle for next event
          triggered = false;
          sprintf(data, SW_MSG, name.c_str(), 1);
          return data;
      }

      /* int val = digitalRead((int)conf["GPIO"]);
      //if on
			if(val == 0){
        sprintf(data, SW_MSG, name.c_str(), val);
        return data;
      } */
      return nullptr;
		}
};


}

#endif