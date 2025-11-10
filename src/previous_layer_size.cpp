#include "previous_layer_size.h"
#include "config.h"

PREVIOUS_LAYER_SIZE::PREVIOUS_LAYER_SIZE(sc_module_name nm)
    :sc_module(nm),
    clk_i("clk_i"),
    save_i("save_i"),
    data_i("data_i"),
    data_o("data_o")
{
    data_o.initialize(0);

    SC_METHOD(save_new_element);
    sensitive << clk_i.pos();   
    SC_METHOD(pass_new_element);
    sensitive << clk_i.pos();
}

PREVIOUS_LAYER_SIZE::~PREVIOUS_LAYER_SIZE()
{
}

void PREVIOUS_LAYER_SIZE::save_new_element() {
    if (save_i) {
        #ifdef PREVIOUS_LAYER_SIZE_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PREVIOUS LAYER SIZE] SAVE PREV LAYER: " << data_i << std::endl;
        #endif
        layer_size = data_i;
    }
}

void PREVIOUS_LAYER_SIZE::pass_new_element() {
    data_o = layer_size;
}
