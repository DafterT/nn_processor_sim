#include "generator.h"

GENERATOR::GENERATOR(sc_module_name nm)
    :sc_module(nm),
    queue_size_i("addr_out_bus_i", COUNT_PC_UNIT)
{
    data_o.initialize(0);
    end_generate_o.initialize(true);
    address_o.initialize(0);
    w_o.initialize(false);

    SC_CTHREAD(generate, clk_i.pos());
}

GENERATOR::~GENERATOR()
{
}

int find_min_index(const sc_vector<sc_in<int>>& queue_size_i) {
    int min_idx = 0;
    int min_val = queue_size_i[0].read();

    for (size_t i = 1; i < queue_size_i.size(); ++i) {
        int v = queue_size_i[i].read();
        if (v < min_val) {
            min_val = v;
            min_idx = static_cast<int>(i);
        }
    }
    return min_idx;
}

void GENERATOR::generate() {
    int counter = 0;
    while(true) {
        switch (state)
        {
        case states::IDLE:
            end_generate_o = true;
            address_o = 0;
            data_o = 0;
            w_o = false;
            counter = 0;
            if (generate_i) {
                end_generate_o = false;
                state = states::GENERATE;
                #ifdef GEN_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [GENERATOR] GENERATOR START" << std::endl;
                #endif
            }
            wait();
            break;
        case states::GENERATE:
            int queue_min_ind = find_min_index(queue_size_i);
            if (queue_size_i[queue_min_ind] < QUEUE_SIZE) {
                #ifdef GEN_DEBUG
                std::cout << "["<< sc_time_stamp() << "] [GENERATOR] GENERATE TO: " << queue_min_ind 
                          << " NUMBER: " << counter << std::endl;
                #endif
                data_o = counter;
                address_o = queue_min_ind;
                w_o = true;
                counter += 1;
                if (counter >= layer_size_i) {
                    state = states::IDLE;
                    #ifdef GEN_DEBUG
                    std::cout << "["<< sc_time_stamp() << "] [GENERATOR] GENERATOR STOP" << std::endl;
                    #endif
                } 
                wait();
                w_o = false;
            } else {
                wait();
                w_o = false;
            }
            break;
        }
    }
}