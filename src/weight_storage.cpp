#include "weight_storage.h"

WEIGHT_STORAGE::WEIGHT_STORAGE(sc_module_name nm)
    :sc_module(nm),
    clk_i("clk_i"),
    addr_io_bus_i("addr_io_bus_i"),
    data_io_bus_i("data_io_bus_i"),
    wr_io_bus_i("wr_io_bus_i"),
    addr_out_bus_i("addr_out_bus_i", COUNT_PC_UNIT),
    data_out_bus_o("data_out_bus_o", COUNT_PC_UNIT)
{
    for (auto& data : data_out_bus_o){
        data.initialize(0);
    }
    SC_METHOD(issue_element);
    sensitive << clk_i.pos();
    SC_METHOD(add_to_memory);
    sensitive << clk_i.pos();
}

WEIGHT_STORAGE::~WEIGHT_STORAGE()
{
}

// Модуль - запись в очередь
void WEIGHT_STORAGE::add_to_memory() {
    if (addr_io_bus_i == 3 && wr_io_bus_i == 1) {
        memory.push_back(data_io_bus_i);
        #ifdef WEIGHT_STORAGE_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [WEIGHT STORAGE] GET WEIGHT: " << data_io_bus_i << std::endl;
        #endif
    }
}
void WEIGHT_STORAGE::issue_element() {
    for (int i = 0; i < COUNT_PC_UNIT; i++) {
        if ((int)memory.size() > addr_out_bus_i[i]) {
            data_out_bus_o[i] = memory[addr_out_bus_i[i]];
        } else {
            data_out_bus_o[i] = 0;
        }
    }
}