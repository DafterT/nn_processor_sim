#ifndef _PREVIOUS_LAYER_SIZE_H
#define _PREVIOUS_LAYER_SIZE_H

#include "systemc.h"

SC_MODULE(PREVIOUS_LAYER_SIZE)
{
    sc_in<bool>     clk_i;
    sc_in<bool>     save_i;
    sc_in<int>    data_i;
    sc_out<int>   data_o;
    
    SC_HAS_PROCESS(PREVIOUS_LAYER_SIZE);
    
    PREVIOUS_LAYER_SIZE(sc_module_name nm);
    ~PREVIOUS_LAYER_SIZE();
    
    // Задача - сохранить новый вес
    void save_new_element();
    // Задача - передать новый элемент
    void pass_new_element();
    
private: 
    int layer_size = 0;
    
};

#endif
