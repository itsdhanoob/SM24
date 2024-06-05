#include <Arduino.h>


struct plantProfile
{
    byte max_tempc =30;
    byte min_tempc =18;
    byte desired_tempc = 22;

    byte max_hum =70;
    byte min_hum =40;
    byte max_hum =60;

    /* data */
};


class omegaPlant
{
private:
    /* data */
public:
    omegaPlant(/* args */);
    ~omegaPlant();
};

omegaPlant::omegaPlant(/* args */)
{
}

omegaPlant::~omegaPlant()
{
}
