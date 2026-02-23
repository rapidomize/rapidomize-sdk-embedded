#ifndef RPZ_SWITCH_H_
#define RPZ_SWITCH_H_

/*Input triggers the Buzzer code*/
#include "peripheral.h" 

namespace rpz{

const PROGMEM char *Switch_tmpl = R"(
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

//https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/  
const int IN1=36; //GPIO36
const int IN2=39; //GPIO39

class Switch: public Peripheral{
    const char * dconf = "{\"GPIO\":36}";

    public: 
    Switch(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
      name = "Switch_";
      name+=seq;
      configure(dconf);

      conf["GPIO"] = seq == 1? 36 : 39;
    }
    char * confpg(){
        char *fr = (char *) malloc(4096);
        const char * pname = name.c_str();
        sprintf(fr, Switch_tmpl, pname, pname, enabled?"checked":"", (int)conf["GPIO"]);
        return fr;
    }
    void init(JsonDocument *jconf) {
      Peripheral::init(jconf);
      if(!enabled) return;

      pinMode((int)conf["GPIO"], INPUT);
    }
    
    char * read(){
      if(!enabled) return nullptr;
      
			if(digitalRead((int)conf["GPIO"])==0){
        Serial.print("GPIO is on");
        Utils::indicate();
        delay(1000);
      }
      return nullptr;
		}
};


}


#endif