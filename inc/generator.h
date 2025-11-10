#ifndef _GENERATOR_H
#define _GENERATOR_H

#include "systemc.h"
#include "config.h"

SC_MODULE(GENERATOR)
{
    sc_in<bool>           clk_i;
    sc_in<int>            layer_size_i;
    sc_in<bool>           generate_i;
    sc_out<bool>          end_generate_o;
    sc_out<int>           address_o;
    sc_out<int>           data_o;
    sc_out<bool>          w_o;
    sc_vector<sc_in<int>> queue_size_i;

    SC_HAS_PROCESS(GENERATOR);
    
    GENERATOR(sc_module_name nm);
    ~GENERATOR();
    
    // Задача - выполнить генерацию
    void generate();
    
private: 
    int layer_size = 0;
    enum class states {IDLE, GENERATE};
    states state = states::IDLE;
};

#endif
