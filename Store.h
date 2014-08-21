#ifndef STORE_H
#define STORE_H

#include "Configuration.h"
#include "Elevator.h"

class Store
{
    public:
    Store(unsigned char store);
    virtual ~Store();
    unsigned int pickPeople(unsigned int how_many, Elevator &elevator);

    protected:
    private:
    
    unsigned char m_store;
    unsigned char m_max_dest;
    unsigned char m_min_dest;
    unsigned int  m_destinations[Configuration::MAX_STORES] = {0};
    boost::mutex  m_mtx;

};

#endif // STORE_H
