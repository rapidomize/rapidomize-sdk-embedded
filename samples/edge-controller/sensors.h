#ifndef RPZ_SESNSORS_H_
#define RPZ_SESNSORS_H_

namespace rpz{

class Sensors{
    public: 
        Sensors(){};
        void init();
        char *read();
    private:
        char data[256];
};

}


#endif /* RPZ_SESNSORS_H_ */
