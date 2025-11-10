#include "previous_layer.h"

PREVIOUS_LAYER::PREVIOUS_LAYER(sc_module_name nm)
    :sc_module(nm),
    addr_PC_bus_i("addr_PC_bus_i", COUNT_PC_UNIT),
    data_PC_bus_o("data_PC_bus_o", COUNT_PC_UNIT)
{
    for (auto& data : data_PC_bus_o){
        data.initialize(0);
    }

    SC_METHOD(add_to_mem);
    sensitive << clk_i.pos();   
    SC_METHOD(issue_element_IO);
    sensitive << clk_i.pos();
    SC_METHOD(issue_element_PC);
    sensitive << clk_i.pos();
    SC_METHOD(clear);
    sensitive << clk_i.pos();
}

PREVIOUS_LAYER::~PREVIOUS_LAYER()
{
}

void PREVIOUS_LAYER::add_to_mem() {
    if (wr_io_bus_io && addr_io_bus_i == 2) {
        memory.push_back(data_io_bus_io);
        #ifdef PREVIOUS_LAYER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PREVIOUS LAYER] GET IN MEM FROM IO: " << data_io_bus_io << std::endl;
        #endif
    }
    if (wr_to_memory_from_mem_i) {
        memory.push_back(data_mem_bus_i);
        #ifdef PREVIOUS_LAYER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PREVIOUS LAYER] GET IN MEM FROM MEM: " << data_mem_bus_i << std::endl;
        #endif
    }
}

void PREVIOUS_LAYER::issue_element_IO() {
    if (wr_to_out_i) {
        if ((int)memory.size() > index_to_out) {
            data_io_bus_io = memory[index_to_out];
            wr_io_bus_io = true;
            #ifdef PREVIOUS_LAYER_DEBUG
            std::cout << "["<< sc_time_stamp() << "] [PREVIOUS LAYER] WRITE TO IO: " << memory[index_to_out] << std::endl;
            #endif
            index_to_out++;
        } else {
            sc_stop();
        }
    }
}

void PREVIOUS_LAYER::issue_element_PC() {
    for (int i = 0; i < COUNT_PC_UNIT; i++) {
        if ((int)memory.size() > addr_PC_bus_i[i]) {
            data_PC_bus_o[i] = memory[addr_PC_bus_i[i]];
        } else {
            data_PC_bus_o[i] = -1;
        }
    }
}

void PREVIOUS_LAYER::clear() {
    if (clr_i) {
        memory.clear();
        index_to_out = 0;
    }
}
