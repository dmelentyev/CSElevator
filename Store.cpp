#include "Store.h"

Store::Store(unsigned char store)
{
    m_store = m_max_dest = m_min_dest = store;
}

Store::~Store()
{
}

unsigned int
Store::pickPeople(unsigned int how_many, Elevator &elevator)
{
    unsigned int selected = 0, picked = 0;
    bool up = elevator.goingUp();
    int idx_inc = up?-1:1;
    unsigned char current_dest = up?m_max_dest:m_min_dest;
    
    while (current_dest >= up?m_store+1:m_min_dest
           && current_dest <= up?m_max_dest:m_store-1
           && selected < how_many)
    {
        
        scoped_lock<boost::mutex> lock(m_mtx);
        if (m_destinations[current_dest] > 0)
        {
            picked = min(m_destinations[current_dest], how_many);
            selected += picked;
            how_many -= picked;
            m_destinations[current_dest] -= picked;
            elevator.registerPassengers(picked, current_dest);
        }
        
        //Adjust min/max_dest
        if (m_destinations[current_dest] == 0)
        {
            m_min_dest = up?m_min_dest:current_dest+idx_inc;
            m_max_dest = up?current_dest+idx_inc:m_max_dest;
        }
        
        // Move to the next destination
        current_dest += idx_inc;
    }
    
    return selected;
};
