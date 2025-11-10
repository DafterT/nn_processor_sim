#include "config.h"
#include "io_module.h"
#include "controller.h"
#include "layers_storage.h"
#include "weight_storage.h"
#include "previous_layer.h"
#include "previous_layer_controller.h"
#include "previous_layer_size.h"
#include "generator.h"
#include "processor.h"
#include "next_layer.h"
#include "end_reducer.h"
#include <cstring>

int sc_main(int argc, char* argv[])
{
    sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
            g_configFile = argv[++i];
        } else if (std::strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            g_pictureFile = argv[++i];
        } else if (std::strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            g_outputFile = argv[++i];
        }
    }
    IO io_module("IO");
    CONTROLLER controller("CONTROLLER");
    LAYERS_STORAGE layers_storage("LAYERS_STORAGE");
    WEIGHT_STORAGE weight_storage("WEIGHT_STORAGE");
    PREVIOUS_LAYER previous_layer("PREVIOUS_LAYER");
    PREVIOUS_LAYER_CONTROLLER previous_layer_controller("PREVIOUS_LAYER_CONTROLLER");
    PREVIOUS_LAYER_SIZE previous_layer_size("PREVIOUS_LAYER_SIZE");
    GENERATOR generator("GENERATOR");
    sc_vector<PROCESSOR> processors{"PROCESSORS"};
    processors.init(COUNT_PC_UNIT, [](const char* name, size_t i){
        return new PROCESSOR(name, static_cast<int>(i));
    });
    NEXT_LAYER next_layer_mem("NEXT_LAYER");
    END_REDUCER end_reducer("END_REDUCER");

    sc_clock clk("clk", sc_time(10, SC_NS));
    // Шина записи / чтения
    sc_signal<int>   addr_io_bus;
    sc_signal<float, SC_MANY_WRITERS> data_io_bus;
    sc_signal<bool,  SC_MANY_WRITERS> wr_io_bus;
    // Очередь слоев
    sc_signal<bool> next_layer;
    sc_signal<int>  data_layer;
    // Память весов в PC
    sc_vector<sc_signal<int>>   addr_weight_bus;
    sc_vector<sc_signal<float>> data_weight_bus;
    // Память для предыдущего слоя
    sc_vector<sc_signal<int>>   addr_prev_layer_bus;
    sc_vector<sc_signal<float>> data_prev_layer_bus;
    // Связь между контроллером предыдущего слоя и предыдущим слоем
    sc_signal<bool> wr_to_mem_from_mem_ctrl_mem_bus;
    sc_signal<bool> wr_to_io_ctrl_mem_bus;
    sc_signal<bool> clr_prev_layer_ctrl_mem_bus;
    // Связь между памятями для передачи
    sc_signal<float> data_from_now_layer_to_next_layer;
    sc_signal<bool>  next_memory_transmit;
    // Связь контроллера и управления предыдущем слоем
    sc_signal<bool> clr_prev_layer_controller_bus;
    sc_signal<bool> start_clone_controller_bus;
    sc_signal<bool> end_clone_controller_bus;
    sc_signal<bool> write_out_controller_bus;
    // Сигнал о сохранении предыдущего слоя
    sc_signal<bool> save_prev_layer_size;
    // Сигнал для генератора о старте преобразования
    sc_signal<bool> start_generator;
    // Сигнал о том, что цикл кончился
    sc_signal<bool> cycle_ended;
    sc_signal<int>  start_weight_adderess;
    // Сигнал от предыдщуего слоя
    sc_signal<int>  previous_data_layer;
    // Генератор
    sc_signal<int>            gen_address;
    sc_signal<int>            gen_data;
    sc_signal<bool>           gen_write;
    sc_signal<bool>           gen_end;
    sc_vector<sc_signal<int>> gen_size;
    // сигналы для памяти результата
    sc_vector<sc_signal<int>>   resmem_addr_bus;
    sc_vector<sc_signal<float>> resmem_data_bus;
    sc_vector<sc_signal<bool>>  resmem_w_bus;
    // Линии для промежуточного элемента
    sc_vector<sc_signal<bool>>  contr_empty_bus;

    // Размерности
    addr_weight_bus.init(COUNT_PC_UNIT);
    data_weight_bus.init(COUNT_PC_UNIT);
    addr_prev_layer_bus.init(COUNT_PC_UNIT);
    data_prev_layer_bus.init(COUNT_PC_UNIT);
    gen_size.init(COUNT_PC_UNIT);
    resmem_addr_bus.init(COUNT_PC_UNIT);
    resmem_data_bus.init(COUNT_PC_UNIT);
    resmem_w_bus.init(COUNT_PC_UNIT);
    contr_empty_bus.init(COUNT_PC_UNIT);

    // Соединения
    io_module.clk_i(clk);
    io_module.addr_o(addr_io_bus);
    io_module.data_io(data_io_bus);
    io_module.wr_io(wr_io_bus);

    controller.clk_i(clk);
    controller.addr_i(addr_io_bus);
    controller.data_i(data_io_bus);
    controller.wr_i(wr_io_bus);
    controller.next_layer_o(next_layer);
    controller.size_of_layer_i(data_layer);
    controller.clr_controller_layer_o(clr_prev_layer_controller_bus);
    controller.write_out_controller_layer_o(write_out_controller_bus);
    controller.start_clone_controller_layer_o(start_clone_controller_bus);
    controller.end_clone_controller_layer_i(end_clone_controller_bus);
    controller.save_prev_layer_size_o(save_prev_layer_size);
    controller.start_generator_o(start_generator);
    controller.cycle_ended_i(cycle_ended);
    controller.start_weight_adderess_o(start_weight_adderess);

    layers_storage.clk_i(clk);
    layers_storage.data_i(data_io_bus);
    layers_storage.addr_i(addr_io_bus);
    layers_storage.wr_i(wr_io_bus);
    layers_storage.next(next_layer);
    layers_storage.data_o(data_layer);
   
    weight_storage.clk_i(clk);
    weight_storage.addr_io_bus_i(addr_io_bus);
    weight_storage.data_io_bus_i(data_io_bus);
    weight_storage.wr_io_bus_i(wr_io_bus);
    weight_storage.addr_out_bus_i(addr_weight_bus);
    weight_storage.data_out_bus_o(data_weight_bus);

    previous_layer.clk_i(clk);
    previous_layer.addr_io_bus_i(addr_io_bus);
    previous_layer.data_io_bus_io(data_io_bus);
    previous_layer.wr_io_bus_io(wr_io_bus);
    previous_layer.wr_to_memory_from_mem_i(wr_to_mem_from_mem_ctrl_mem_bus);
    previous_layer.wr_to_out_i(wr_to_io_ctrl_mem_bus);
    previous_layer.clr_i(clr_prev_layer_ctrl_mem_bus);
    previous_layer.addr_PC_bus_i(addr_prev_layer_bus);
    previous_layer.data_PC_bus_o(data_prev_layer_bus);
    previous_layer.data_mem_bus_i(data_from_now_layer_to_next_layer);

    previous_layer_controller.clk_i(clk);
    previous_layer_controller.wr_to_memory_from_mem_o(wr_to_mem_from_mem_ctrl_mem_bus);
    previous_layer_controller.wr_to_out_o(wr_to_io_ctrl_mem_bus);
    previous_layer_controller.clr_o(clr_prev_layer_ctrl_mem_bus);
    previous_layer_controller.next_o(next_memory_transmit);
    previous_layer_controller.clr_i(clr_prev_layer_controller_bus);
    previous_layer_controller.start_clone_i(start_clone_controller_bus);
    previous_layer_controller.end_clone_o(end_clone_controller_bus);
    previous_layer_controller.write_out_i(write_out_controller_bus);
    previous_layer_controller.layer_size_i(data_layer);

    previous_layer_size.clk_i(clk);
    previous_layer_size.save_i(save_prev_layer_size);
    previous_layer_size.data_i(data_layer);
    previous_layer_size.data_o(previous_data_layer);

    generator.clk_i(clk);
    generator.layer_size_i(data_layer);
    generator.generate_i(start_generator);
    generator.end_generate_o(gen_end);
    generator.address_o(gen_address);
    generator.data_o(gen_data);
    generator.w_o(gen_write);
    generator.queue_size_i(gen_size);

    for (size_t i = 0; i < processors.size(); ++i) {
        auto& p = processors[i];

        p.clk_i(clk);
        // Шина для очереди (общая от генератора)
        p.queue_address_i(gen_address);
        p.queue_data_i(gen_data);
        p.queue_w_i(gen_write);
        p.queue_size_o(gen_size[i]);
        // Шина для процессора
        p.contr_start_weight_address_i(start_weight_adderess);
        p.contr_empty_o(contr_empty_bus[i]);
        // Шина для памяти текущего слоя (пока нет промежуточного элемента)
        p.resmem_addr_o(resmem_addr_bus[i]);
        p.resmem_data_o(resmem_data_bus[i]);
        p.resmem_w_o(resmem_w_bus[i]);
        // Шина для памяти предыдущего слоя
        p.prevmem_addr_o(addr_prev_layer_bus[i]);
        p.prevmem_data_i(data_prev_layer_bus[i]);
        p.prevmem_size_i(previous_data_layer);
        // Шина для памяти весов
        p.weight_addr_o(addr_weight_bus[i]);
        p.weight_data_i(data_weight_bus[i]);
    }

    next_layer_mem.clk_i(clk);
    next_layer_mem.next_i(next_memory_transmit);                  
    next_layer_mem.data_o(data_from_now_layer_to_next_layer); 
    next_layer_mem.addr_i.bind(resmem_addr_bus);
    next_layer_mem.data_i.bind(resmem_data_bus);
    next_layer_mem.w_i.bind(resmem_w_bus);

    end_reducer.gen_end_i(gen_end);
    end_reducer.contr_empty_i.bind(contr_empty_bus);
    end_reducer.cycle_ended_o(cycle_ended);

    sc_trace_file *wf = sc_create_vcd_trace_file("wave");

    // === Тактирование ===
    sc_trace(wf, clk, "top.clk");

    // === IO-шина ===
    sc_trace(wf, addr_io_bus, "io.addr");
    sc_trace(wf, data_io_bus, "io.data");
    sc_trace(wf, wr_io_bus,   "io.wr");

    // === Контроллер, размеры слоев и завершение цикла ===
    sc_trace(wf, next_layer,                 "controller.next_layer");
    sc_trace(wf, data_layer,                 "layers.current_size");
    sc_trace(wf, previous_data_layer,        "prev_layer_size.value");
    sc_trace(wf, start_weight_adderess,      "controller.start_weight_address");
    sc_trace(wf, save_prev_layer_size,       "controller.save_prev_layer_size");
    sc_trace(wf, start_generator,            "controller.start_generator");
    sc_trace(wf, cycle_ended,                "end_reducer.cycle_ended");

    // === Управление предыдущим слоем ===
    sc_trace(wf, clr_prev_layer_controller_bus, "controller.clr_prev_layer_ctrl");
    sc_trace(wf, write_out_controller_bus,      "controller.write_out_prev_layer");
    sc_trace(wf, start_clone_controller_bus,    "controller.start_clone_prev_layer");
    sc_trace(wf, end_clone_controller_bus,      "prev_layer_ctrl.end_clone");

    // === Контроллер предыдущего слоя ↔ Память предыдущего слоя ===
    sc_trace(wf, wr_to_mem_from_mem_ctrl_mem_bus, "prev_layer_ctrl.wr_to_memory");
    sc_trace(wf, wr_to_io_ctrl_mem_bus,           "prev_layer_ctrl.wr_to_io");
    sc_trace(wf, clr_prev_layer_ctrl_mem_bus,     "prev_layer_ctrl.clr");
    sc_trace(wf, next_memory_transmit,            "prev_layer_ctrl.next");

    // === Память текущего/следующего слоя ↔ Предыдущий слой ===
    sc_trace(wf, data_from_now_layer_to_next_layer, "next_layer_mem.data_out");

    // === Генератор ===
    sc_trace(wf, gen_address, "generator.addr");
    sc_trace(wf, gen_data,    "generator.data");
    sc_trace(wf, gen_write,   "generator.w");
    sc_trace(wf, gen_end,     "generator.end");
    for (size_t i = 0; i < gen_size.size(); ++i) {
        std::string n = "generator.queue_size(" + std::to_string(i) + ")";
        sc_trace(wf, gen_size[i], n.c_str());
    }

    // === Шины весов к процессорам ===
    for (size_t i = 0; i < addr_weight_bus.size(); ++i) {
        std::string n1 = "weights.addr_to_proc(" + std::to_string(i) + ")";
        std::string n2 = "weights.data_to_proc(" + std::to_string(i) + ")";
        sc_trace(wf, addr_weight_bus[i], n1.c_str());
        sc_trace(wf, data_weight_bus[i], n2.c_str());
    }

    // === Шины памяти предыдущего слоя к/от процессоров ===
    for (size_t i = 0; i < addr_prev_layer_bus.size(); ++i) {
        std::string n1 = "prev_layer.addr_from_proc(" + std::to_string(i) + ")";
        std::string n2 = "prev_layer.data_to_proc(" + std::to_string(i) + ")";
        sc_trace(wf, addr_prev_layer_bus[i], n1.c_str());
        sc_trace(wf, data_prev_layer_bus[i], n2.c_str());
    }

    // === Шины памяти результата (текущего/следующего слоя) от процессоров ===
    for (size_t i = 0; i < resmem_addr_bus.size(); ++i) {
        std::string n1 = "next_layer_mem.addr_from_proc(" + std::to_string(i) + ")";
        std::string n2 = "next_layer_mem.data_from_proc(" + std::to_string(i) + ")";
        std::string n3 = "next_layer_mem.w_from_proc(" + std::to_string(i) + ")";
        sc_trace(wf, resmem_addr_bus[i], n1.c_str());
        sc_trace(wf, resmem_data_bus[i], n2.c_str());
        sc_trace(wf, resmem_w_bus[i],    n3.c_str());
    }

    // === Статусы процессоров ===
    for (size_t i = 0; i < contr_empty_bus.size(); ++i) {
        std::string n = "proc(" + std::to_string(i) + ").contr_empty";
        sc_trace(wf, contr_empty_bus[i], n.c_str());
    }

    sc_start();
    sc_close_vcd_trace_file(wf);
    
    #ifdef PROCESSOR_STAT
    for (size_t i = 0; i < processors.size(); ++i) {
        std::cout << "PROCESSOR CALCULATED [" << i << "]" << 
            processors[i].count_neurons_calc << " NEURONS" << std::endl;
    }
    #endif

    return(0);
}
