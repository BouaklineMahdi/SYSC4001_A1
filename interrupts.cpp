/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include<interrupts.hpp>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    const int CONTEXT_SAVE_TIME = 10;
    const int ISR_ACTIVITY_TIME = 40;
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
        // NOTE: There is a discrepency in the assignment description and what was posted by the professor as an example. The assignment notes
        // take into account and print IRET, get ISR address, etc... For simplicity, and based on what we found to be most logical, we have decided to go with
        // the most recent instructions/examples we were given, which did not include "variables". Additionally, at 1ms, the instructions state they are negligable.
        // Generally, whenever we were uncertain with a requirement, we decided to comply with what the professor's example implemented.

        if (activity == "CPU") {
            // CPU burst
            execution += std::to_string(current_time) + ", " + 
                        std::to_string(duration_intr) + ", CPU Burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            int device_num = duration_intr;
            int device_delay = delays[device_num];
            
            // Calling helper function
            auto [boilerplate_output, updated_time] = intr_boilerplate(current_time, device_num, CONTEXT_SAVE_TIME, vectors);
            execution += boilerplate_output;
            current_time = updated_time;
            
            // Breaking into 3 ISR activities that sum to device_delay
            // Abiding by the professor's example
            int first = ISR_ACTIVITY_TIME;
            int second = ISR_ACTIVITY_TIME;
            int third = device_delay - first - second;
            
            execution += std::to_string(current_time) + ", " + std::to_string(first) + 
                        ", SYSCALL: run the ISR (device driver)\n";
            current_time += first;
            
            execution += std::to_string(current_time) + ", " + std::to_string(second) + 
                        ", transfer data from device to memory\n";
            current_time += second;
            
            execution += std::to_string(current_time) + ", " + std::to_string(third) + 
                        ", check for errors\n";
            current_time += third;    
        }
        else if (activity == "END_IO") {
            int device_num = duration_intr;
            int device_delay = delays[device_num];
            
            auto [boilerplate_output, updated_time] = intr_boilerplate(current_time, device_num, CONTEXT_SAVE_TIME, vectors);
            execution += boilerplate_output;
            current_time = updated_time;
            
            // Breaking into 2 ISR activities that sum to device_delay
            // Abiding by the professor's example
            int first = ISR_ACTIVITY_TIME;
            int second = device_delay - first;
            
            execution += std::to_string(current_time) + ", " + std::to_string(first) + 
                        ", ENDIO: run the ISR (device driver)\n";
            current_time += first;
            
            execution += std::to_string(current_time) + ", " + std::to_string(second) + 
                        ", check device status\n";
            current_time += second;
        }
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
