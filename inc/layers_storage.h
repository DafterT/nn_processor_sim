#ifndef _LAYERS_STORAGE_H
#define _LAYERS_STORAGE_H

#include "systemc.h"
#include <vector>

SC_MODULE(LAYERS_STORAGE)
{
    sc_in<bool>  clk_i;
    // Интерфейс шины
    sc_in<int>   addr_i;
    sc_in<float> data_i;
    sc_in<bool>  wr_i;
    // Интерфейс очереди
    sc_in<bool> next;
    sc_out<int> data_o;
    
    SC_HAS_PROCESS(LAYERS_STORAGE);
    
    LAYERS_STORAGE(sc_module_name nm);
    ~LAYERS_STORAGE();

private: 
    std::vector<int> memory;
    int index = 0;

    // Модуль - следующий элемент очереди
    void next_element();
    // Модуль - запись в очередь
    void add_to_queue();
    // Модуль - выдача значения из очереди
    void issue_element();
};


#endif
