#ifndef _PREVIOUS_LAYER_H
#define _PREVIOUS_LAYER_H

#include "systemc.h"
#include "config.h"
#include <vector>

SC_MODULE(PREVIOUS_LAYER)
{
    sc_in<bool>   clk_i;
    // Интерфейс шины
    sc_in<int>       addr_io_bus_i;
    sc_inout<float>  data_io_bus_io;
    sc_inout<bool>   wr_io_bus_io;
    // Интерфейс в контроллер
    sc_in<bool> wr_to_memory_from_mem_i;
    sc_in<bool> wr_to_out_i;
    sc_in<bool> clr_i;
    // Интерфейс выходной из памяти
    sc_vector<sc_in<int>>       addr_PC_bus_i;
    sc_vector<sc_out<float>>    data_PC_bus_o;
    // Интерфейс для передачи значений памяти
    sc_in<float>                data_mem_bus_i;
    
    SC_HAS_PROCESS(PREVIOUS_LAYER);
    
    PREVIOUS_LAYER(sc_module_name nm);
    ~PREVIOUS_LAYER();

private: 
    std::vector<float> memory;
    int index_to_out = 0;
    // Модуль - запись в память
    void add_to_mem();
    // Модуль - выдача значения из памяти в IO
    void issue_element_IO();
    // Модуль - выдача значения из памяти в процессор
    void issue_element_PC();
    // Модуль - отчистка памяти
    void clear();
};

#endif
