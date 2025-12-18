#ifndef RPZ_SESNSOR_H_
#define RPZ_SESNSOR_H_

namespace rpz{

class Sensor{
    public: 
        Sensor(){};
        void init();
        char *read();
        void *write(char *);
        void *exec(char *);
    protected:
        char data[256];
};

}


#endif /* RPZ_SESNSOR_H_ */
