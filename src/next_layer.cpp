#include "next_layer.h"
#include "config.h"

NEXT_LAYER::NEXT_LAYER(sc_module_name nm)
    :sc_module(nm),
    addr_i("addr_i", COUNT_PC_UNIT),
    data_i("data_i", COUNT_PC_UNIT),
    w_i("w_i", COUNT_PC_UNIT)
{
    memory.reserve(MEM_SIZE);

    SC_METHOD(issue_element);
    sensitive << clk_i.pos();
    SC_METHOD(add_to_memory);
    sensitive << clk_i.pos();
}

NEXT_LAYER::~NEXT_LAYER()
{
}

// Модуль - запись в очередь
void NEXT_LAYER::add_to_memory() {
    for (int i = 0; i < COUNT_PC_UNIT; i++) {
        if (w_i[i]) {
            memory[addr_i[i]] = data_i[i];
            #ifdef NEXT_LAYER_DEBUG
            std::cout << "["<< sc_time_stamp() << "] [NEXT LAYER] [" <<
                    i <<"] SAVE: " << data_i[i] 
                    << " AT ADDRESS: " << addr_i[i]
                    << std::endl;
            #endif
            index = 0;
        }
    }
}

void NEXT_LAYER::issue_element() {
    if (next_i) {
        data_o = memory[index];
        #ifdef NEXT_LAYER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [NEXT LAYER] TRANSMITT: " << memory[index] 
            << " AT ADDRESS: " << index
            << std::endl;
        #endif
        index += 1;
    }
}
