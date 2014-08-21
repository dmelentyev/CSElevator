#ifndef PERSON_H
#define PERSON_H


class Person
{
    public:
        Person();
        virtual ~Person();
        unsigned int GetWorkingFloor() { return m_WorkingFloor; }
        void SetWorkingFloor(unsigned int val) { m_WorkingFloor = val; }
    protected:
    private:
        unsigned int m_WorkingFloor;
};

#endif // PERSON_H
