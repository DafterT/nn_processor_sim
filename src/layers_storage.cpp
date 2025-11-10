#include "layers_storage.h"
#include "config.h"

LAYERS_STORAGE::LAYERS_STORAGE(sc_module_name nm)
    :sc_module(nm),
    clk_i("clk_i"),
    addr_i("addr_i"),
    data_i("data_i"),
    wr_i("wr_i"),
    next("next"),
    data_o("data_o")
{
    data_o.initialize(0);

    SC_METHOD(next_element);
    sensitive << clk_i.pos();
    SC_METHOD(add_to_queue);
    sensitive << clk_i.pos();
    SC_METHOD(issue_element);
    sensitive << clk_i.pos();
}

LAYERS_STORAGE::~LAYERS_STORAGE()
{
}

// Модуль - следующий элемент очереди
void LAYERS_STORAGE::next_element() {
    if (next) {
        index += 1;
        #ifdef LAYER_STORAGE_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [LAYER STORAGE] NEXT LAYER" << std::endl;
        #endif
    }
}
// Модуль - запись в очередь
void LAYERS_STORAGE::add_to_queue() {
    if (addr_i == 1 && wr_i == 1) {
        memory.push_back((int)data_i);
        #ifdef LAYER_STORAGE_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [LAYER STORAGE] GET LAYER: " << (int)data_i << std::endl;
        #endif
    }
}
// Модуль - выдача значения из очереди
void LAYERS_STORAGE::issue_element() {
    if ((int)memory.size() > index) {
        data_o = memory[index];
    } else {
        data_o = 0;
    }
}
