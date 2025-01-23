#include "nes/display.hh"
#include "nes/nes.hh"
#include "nes/util/rgb.hh"
#include "nes/util/snapshot.hh"
#include <iostream>

namespace
{
	class dummy_display final : public nes::display
	{
		std::uint8_t buffer_a_[width * height * 3]{};
		std::uint8_t buffer_b_[width * height * 3]{};
		std::uint8_t* front_buffer_{ buffer_a_ };
		std::uint8_t* back_buffer_{ buffer_b_ };

	public:
		explicit dummy_display() = default;

		auto get(unsigned const x, unsigned const y) const -> nes::rgb override
		{
			auto const offset = ((y * width) + x) * 3;
			return nes::rgb{ back_buffer_[offset + 0], back_buffer_[offset + 1], back_buffer_[offset + 2] };
		}

		auto set(unsigned const x, unsigned const y, nes::rgb const value) -> void override
		{
			auto const offset = ((y * width) + x) * 3;
			back_buffer_[offset + 0] = value.r;
			back_buffer_[offset + 1] = value.g;
			back_buffer_[offset + 2] = value.b;
		}

		auto switch_buffers() -> void override
		{
			std::swap(front_buffer_, back_buffer_);
		}
	};
} // namespace

int main()
{
	auto const path = "/Users/hannes/Documents/temp/supermario.nes";

	auto cartridge = nes::cartridge::from_file(path);
	if (!cartridge.is_valid())
	{
		std::cerr << "Could not load cartridge!" << std::endl;
		std::abort();
	}

	// Initialize the system to be tested.
	auto display_a = std::make_unique<dummy_display>();
	auto sys_a = nes::nes{ cartridge, *display_a };

	// Initialize the (assumed to be working) reference implementation.
	auto display_b = std::make_unique<dummy_display>();
	auto sys_b = nes::nes{ cartridge, *display_b };

	// Run.
	while (true)
	{
		auto const status_a = sys_a.get_snapshot();
		auto const status_b = sys_b.get_snapshot();

		if (status_a != status_b)
		{
			std::cout << "Different snapshot!\n";
			return 1;
		}

		std::cout << "Cycle: " << status_a.cpu_cycle.to_cpu() << ", " << status_b.cpu_cycle.to_cpu() << '\n';

		sys_a.step_to_nmi();
		sys_b.step_to_nmi();
	}
}