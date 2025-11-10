#ifndef _NEXT_LAYER_H
#define _NEXT_LAYER_H

#include "systemc.h"
#include "config.h"
#include <vector>

SC_MODULE(NEXT_LAYER)
{
    sc_in<bool>   clk_i;
    // Интерфейс шины входной
    sc_vector<sc_in<int>>   addr_i;
    sc_vector<sc_in<float>> data_i;
    sc_vector<sc_in<bool>>  w_i;
    // Интерфейс шины в память
    sc_in<bool>   next_i;
    sc_out<float> data_o;
    
    SC_HAS_PROCESS(NEXT_LAYER);
    
    NEXT_LAYER(sc_module_name nm);
    ~NEXT_LAYER();

private: 
    std::vector<float> memory;
    int index = 0;
    // Модуль - запись в память
    void add_to_memory();
    // Модуль - выдача значения из памяти
    void issue_element();
};

#endif
