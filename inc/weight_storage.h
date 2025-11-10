#ifndef _WEIGHT_STORAGE_H
#define _WEIGHT_STORAGE_H

#include "systemc.h"
#include "config.h"
#include <vector>

SC_MODULE(WEIGHT_STORAGE)
{
    sc_in<bool>   clk_i;
    // Интерфейс шины входной
    sc_in<int>    addr_io_bus_i;
    sc_in<float>  data_io_bus_i;
    sc_in<bool>   wr_io_bus_i;
    // Интерфейс шины выходной в PC
    sc_vector<sc_in<int>>       addr_out_bus_i;
    sc_vector<sc_out<float>>    data_out_bus_o;
    
    SC_HAS_PROCESS(WEIGHT_STORAGE);
    
    WEIGHT_STORAGE(sc_module_name nm);
    ~WEIGHT_STORAGE();

private: 
    std::vector<float> memory;

    // Модуль - запись в память
    void add_to_memory();
    // Модуль - выдача значения из памяти
    void issue_element();
};


#endif
