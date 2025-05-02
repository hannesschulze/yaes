#include <iostream>
#include <cstdlib>
#include <thread>
#include "serial-controller.hh"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage:\n";
		std::cerr << "  " << argv[0] << " <controller-path>" << std::endl;
		return EXIT_FAILURE;
    }

    auto const path = argv[1];
    auto controller = nes::app::test::serial_controller{ path };
    if (!controller.is_reliable())
    {
        return EXIT_FAILURE;
    }

    while (true)
    {
        auto const buttons = controller.read_buttons();
        if (!buttons.is_empty())
        {
            std::cout << "Buttons: " << buttons.get_raw_value() << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    return EXIT_SUCCESS;
}
