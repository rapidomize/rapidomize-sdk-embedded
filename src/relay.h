#ifndef RPZ_RELAY_H_
#define RPZ_RELAY_H_


/*Relay output code*/
#include "peripheral.h" 

namespace rpz{

const char *Relay_tmpl = R"(
<div  class="card">
    <div class="row pos-r"><h3>%s</h3><input type="checkbox" name="%s" %s  class="pos-a" style="right: 0px;"></div>
    <table>
        <tr><td>GPIO</td><td>
         <input type="number" name="GPIO" value="%d" disabled>
        <!--<select name="GPIO" value="1">
            <option value="2">1</option>
            <option value="15">2</option>
        </select>--></td>
    </table> 
</div>  
)"; 

const char *RL_MSG  = R"({"%s":%d})";

//https://rapidomize.com/docs/solutions/iot/device/rpz-d2x2t2ux-we/
const int R1 = 2;  //GPIO2
const int R2 = 15; //GPI015

class Relay: public Peripheral{

  const char * dconf = "{\"GPIO\":2}";

  public: 
    Relay(Preferences *prefs, int seq=1):Peripheral(prefs,seq){
      name = "Relay_";
      name+=seq;
      configure(dconf);

      conf["GPIO"] = seq == 1? 2 : 15;
    }
    char * confpg(){
        char *fr = (char *) malloc(4096);
        sprintf(fr, Relay_tmpl, name.c_str(),name.c_str(), enabled?"checked":"", (int)conf["GPIO"]);
        return fr;
    }
    void init(JsonDocument *jconf) {
      Peripheral::init(jconf);
      if(!enabled) return;

      pinMode((int)conf["GPIO"], OUTPUT);
    }

    void *write(char *data){
      if(!enabled) return nullptr;
      
      digitalWrite((int)conf["GPIO"],HIGH);
      delay(100); 
      digitalWrite((int)conf["GPIO"],LOW) ;
      delay(100); 
      return nullptr;
    }
};

}

#endif 