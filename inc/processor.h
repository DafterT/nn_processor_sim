#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include "systemc.h"
#include "config.h"
#include <vector>

SC_MODULE(PROCESSOR)
{
    sc_in<bool>           clk_i;
    // Шина для очреди
    sc_in<int>  queue_address_i;
    sc_in<int>  queue_data_i;
    sc_in<bool> queue_w_i;
    sc_out<int> queue_size_o;
    // Шина для процессора
    sc_out<bool> contr_empty_o;
    sc_in<int>   contr_start_weight_address_i;
    // Шина для памяти текущего слоя
    sc_out<int>   resmem_addr_o;
    sc_out<float> resmem_data_o;
    sc_out<bool>  resmem_w_o;
    // Шина для памяти предыдущего слоя
    sc_out<int>  prevmem_addr_o;
    sc_in<float> prevmem_data_i; 
    // Шина для памяти весов
    sc_out<int>  weight_addr_o;
    sc_in<float> weight_data_i; 
    // Шина для размера предыдущего слоя
    sc_in<int>   prevmem_size_i;

    SC_HAS_PROCESS(PROCESSOR);
    
    PROCESSOR(sc_module_name nm, int address_);
    ~PROCESSOR();
    
    // Задача - выполнить генерацию
    void process();
    void add_to_queue();
    
    #ifdef PROCESSOR_STAT
    int count_neurons_calc = 0;
    #endif
private: 
    int address = -1;
    bool is_working = false;
    std::vector<int> queue_nn_number;
};

#endif
