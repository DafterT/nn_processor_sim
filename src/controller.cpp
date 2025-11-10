#include "controller.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <vector>

CONTROLLER::CONTROLLER(sc_module_name nm)
    :sc_module(nm),
    clk_i("clk_i"),
    addr_i("addr_i"),
    data_i("data_i"),
    wr_i("wr_i"),
    next_layer_o("next_layer_o"),
    size_of_layer_i("size_of_layer_i"),
    clr_controller_layer_o("clr_controller_layer_o"),
    write_out_controller_layer_o("write_out_controller_layer_o"),
    start_clone_controller_layer_o("start_clone_controller_layer_o"),
    end_clone_controller_layer_i("end_clone_controller_layer_i"),
    save_prev_layer_size_o("save_prev_layer_size_o"),
    start_generator_o("start_generator_o"),
    cycle_ended_i("cycle_ended_i"),
    start_weight_adderess_o("start_weight_adderess_o")
{ 
    next_layer_o.initialize(false);
    clr_controller_layer_o.initialize(false);
    write_out_controller_layer_o.initialize(false);
    start_clone_controller_layer_o.initialize(false);
    save_prev_layer_size_o.initialize(false);
    start_generator_o.initialize(false);
    start_weight_adderess_o.initialize(0);

    SC_CTHREAD(process, clk_i.pos());

    SC_METHOD(state_read_data);
    sensitive << clk_i.pos();

    SC_METHOD(state_start_conv);
    sensitive << clk_i.pos();
}

CONTROLLER::~CONTROLLER()
{
}

void CONTROLLER::state_read_data() {
    if (addr_i == 0 && 
        data_i == INFINITY && 
        wr_i && 
        state == states::IDLE) {
        state = states::READ_DATA;
        #ifdef CONTROLLER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] READ DATA" << std::endl;
        #endif
    }
}

void CONTROLLER::state_start_conv() {
    if (addr_i == 0 && 
        data_i == -INFINITY && 
        wr_i && 
        state == states::IDLE) {
        state = states::START_CONV;
        #ifdef CONTROLLER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] START CONV" << std::endl;
        #endif
    }
}

void CONTROLLER::process() {
    int prev_layer = 0;
    while (true)
    {
        switch (state)
        {
        case states::IDLE:
            start_weight_adderess_o = 0;
            wait();
            break;
        case states::READ_DATA:
            wait();
            if (addr_i == 0 && wr_i) {
                layers = data_i;
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] GET LAYES: " << layers << std::endl;
                #endif
                state = states::IDLE;
            }
            break;
        case states::START_CONV:
            #ifdef CONTROLLER_DEBUG
            std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] START CONROL"<< std::endl;
            #endif
            for (int i = 0; i < layers - 1; i++) {
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] CALC LAYER "<< i + 1 << std::endl;
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] SAVE PREV" << std::endl;
                #endif
                save_prev_layer_size_o = true;
                wait();
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] GET NEXT LAYER" << std::endl;
                #endif
                save_prev_layer_size_o = false;
                prev_layer = size_of_layer_i;
                next_layer_o = true;
                wait();
                next_layer_o = false;
                start_generator_o = true;
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] START GENERATOR" << std::endl;
                #endif
                wait();
                start_generator_o = false;
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] WAIT END CLK" << std::endl;
                #endif
                wait(); wait();
                while (!cycle_ended_i) wait();
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] CLEAR CONTROLLER LAYER" << std::endl;
                #endif
                clr_controller_layer_o = true;
                wait();
                clr_controller_layer_o = false;
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] START CLONE" << std::endl;
                #endif
                start_clone_controller_layer_o = true;
                wait();
                start_clone_controller_layer_o = false;
                while(!end_clone_controller_layer_i) wait();
                #ifdef CONTROLLER_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [CONTROLLER] END CLONE" << std::endl;
                #endif
                start_weight_adderess_o = (prev_layer + 1) * size_of_layer_i + start_weight_adderess_o; 
            }
            write_out_controller_layer_o = true;
            state = states::IDLE;
            break;
        default:
            break;
        }
    }    
}
