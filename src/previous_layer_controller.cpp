#include "previous_layer_controller.h"
#include "config.h"

PREVIOUS_LAYER_CONTROLLER::PREVIOUS_LAYER_CONTROLLER(sc_module_name nm)
    :sc_module(nm),
    clk_i("clk_i"),
    // Интерфейс в предыдущий слой
    wr_to_memory_from_mem_o("wr_to_memory_from_mem_o"),
    wr_to_out_o("wr_to_out_o"),
    clr_o("clr_o"),
    // Интерфейс в текущий слой
    next_o("next_o"),
    // Интерфейс для общения с контроллером
    clr_i("clr_i"),
    start_clone_i("start_clone_i"),
    end_clone_o("end_clone_o"),
    write_out_i("write_out_i"),
    layer_size_i("layer_size_i")
{
    wr_to_out_o.initialize(0);
    wr_to_memory_from_mem_o.initialize(0);
    clr_o.initialize(0);
    next_o.initialize(0);
    end_clone_o.initialize(0);
  
    SC_METHOD(write_out_to_IO);
    sensitive << clk_i.pos();
    SC_CTHREAD(copy_memory, clk_i.pos());
    SC_METHOD(clear);
    sensitive << clk_i.pos();
}

PREVIOUS_LAYER_CONTROLLER::~PREVIOUS_LAYER_CONTROLLER()
{
}

// Модуль - отчистка
void PREVIOUS_LAYER_CONTROLLER::clear() {
    clr_o = clr_i;
}
// Модуль - вывод из памяти в IO
void PREVIOUS_LAYER_CONTROLLER::write_out_to_IO() {
    wr_to_out_o = write_out_i;
}
// Модуль - клонирование между памятями
void PREVIOUS_LAYER_CONTROLLER::copy_memory() {
    while (true) {
        while(!start_clone_i) {
            end_clone_o = false;
            wait();
        }
        #ifdef PREV_LAYER_CONTROLLER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PREV LAYER CONTROLLER]" 
                <<" START CLONE"
                << std::endl;
        #endif
        // Пришел сигнал начать клонировать
        next_o = true;
        wait();
        for (int i = 0; i < layer_size_i; i++) {
            next_o = i != layer_size_i - 1;
            wr_to_memory_from_mem_o = true;
            wait();
        }
        next_o = false;
        wr_to_memory_from_mem_o = false;
        end_clone_o = true;
        #ifdef PREV_LAYER_CONTROLLER_DEBUG
        std::cout << "["<< sc_time_stamp() << "] [PREV LAYER CONTROLLER]" 
                <<" END CLONE"
                << std::endl;
        #endif
        wait();
    }
}
