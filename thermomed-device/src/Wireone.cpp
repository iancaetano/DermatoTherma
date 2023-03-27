# include "Wireone.h"


#define SCL_wireone                 PA9
#define SDA_wireone                 PA8

void begin_I2C_wireone(){
    wireOne.setSDA(SDA_wireone);
    wireOne.setSCL(SCL_wireone);
    wireOne.begin();
}
