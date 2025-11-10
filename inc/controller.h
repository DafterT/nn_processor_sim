#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "systemc.h"

SC_MODULE(CONTROLLER)
{
    sc_in<bool>  clk_i;
    // Шина вход-выход
    sc_in<int>   addr_i;
    sc_in<float> data_i;
    sc_in<bool>  wr_i;
    // Связь с очередью слоев
    sc_out<bool> next_layer_o;
    sc_in<int>   size_of_layer_i;
    // Связь с контроллером предыдущего слоя
    sc_out<bool> clr_controller_layer_o;
    sc_out<bool> write_out_controller_layer_o;
    sc_out<bool> start_clone_controller_layer_o;
    sc_in<bool>  end_clone_controller_layer_i;
    // Сохранить предыдущий размер
    sc_out<bool> save_prev_layer_size_o;
    // Start generator
    sc_out<bool> start_generator_o;
    // Цикл кончился
    sc_in<bool>  cycle_ended_i;
    // Начальный адрес весов
    sc_out<int>  start_weight_adderess_o;
    
    SC_HAS_PROCESS(CONTROLLER);
    
    CONTROLLER(sc_module_name nm);
    ~CONTROLLER();

private: 
    enum class states {IDLE, READ_DATA, START_CONV};
    states state = states::IDLE;
    int layers = 0;
    // Модуль - смена состояния
    void state_read_data();
    void state_start_conv();
    // Задача - основной цикл работы системы
    void process();
};


#endif
