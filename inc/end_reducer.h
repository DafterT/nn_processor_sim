#ifndef _END_REDUCER_H
#define _END_REDUCER_H

#include "systemc.h"
#include "config.h"

SC_MODULE(END_REDUCER)
{
    // Входы
    sc_in<bool>                 gen_end_i;       // от генератора
    sc_vector< sc_in<bool> >    contr_empty_i;   // от процессоров

    // Выход
    sc_out<bool>                cycle_ended_o;   // в контроллер

    SC_HAS_PROCESS(END_REDUCER);

    END_REDUCER(sc_module_name nm);
    ~END_REDUCER();

private:
    void reduce();
};

#endif
