#ifndef _PREVIOUS_LAYER_CONTROLLER_H
#define _PREVIOUS_LAYER_CONTROLLER_H

#include "systemc.h"
#include "config.h"
#include <vector>

SC_MODULE(PREVIOUS_LAYER_CONTROLLER)
{
    sc_in<bool> clk_i;
    // Интерфейс в предыдущий слой
    sc_out<bool> wr_to_memory_from_mem_o;
    sc_out<bool> wr_to_out_o;
    sc_out<bool> clr_o;
    // Интерфейс в текущий слой
    sc_out<bool> next_o; 
    // Интерфейс для общения с контроллером
    sc_in<bool>  clr_i;
    sc_in<bool>  start_clone_i;
    sc_out<bool> end_clone_o;
    sc_in<bool>  write_out_i;
    // Интерфейс для получения размера текущего слоя
    sc_in<int> layer_size_i;

    SC_HAS_PROCESS(PREVIOUS_LAYER_CONTROLLER);
    
    PREVIOUS_LAYER_CONTROLLER(sc_module_name nm);
    ~PREVIOUS_LAYER_CONTROLLER();

private: 
    // Модуль - отчистка
    void clear();
    // Модуль - вывод из памяти в IO
    void write_out_to_IO();
    // Модуль - клонирование между памятями
    void copy_memory();
};


#endif
