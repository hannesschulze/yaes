#include "serial-controller.hh"
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

namespace nes::app::test
{
	serial_controller::serial_controller(char const* path)
		: path_{ path }
	{
		fd_ = open(path, O_RDONLY | O_NOCTTY | O_NDELAY | O_NONBLOCK);
		if (fd_ == -1)
		{
			perror("serial_controller: open");
			return;
		}

		if (!set_up()) { close(); }
	}

	serial_controller::~serial_controller()
	{
		close();
	}

	auto serial_controller::is_reliable() const -> bool
	{
		return fd_ != -1;
	}

	auto serial_controller::read_buttons() -> sys::button_mask
	{
		if (fd_ == -1) { return sys::button_mask{}; }

		// Poll for any updates until we can't read anymore.
		while (true)
		{
			auto buf = u8{};
			auto const res = read(fd_, &buf, 1);
			if (res == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK) { break; }

				perror("read from controller");
				close();
				break;
			}

			current_ = sys::button_mask::from_raw_value(buf);
		}

		return current_;
	}


	auto serial_controller::set_up() -> bool
	{
		if (ioctl(fd_, TIOCEXCL) == -1)
		{
			perror("serial_controller: ioctl");
			return false;
		}

		auto attr = termios{};
		if (tcgetattr(fd_, &attr) == -1)
		{
			perror("serial_controller: tcgetattr");
			return false;
		}

		cfsetispeed(&attr, B57600);
		cfsetospeed(&attr, B57600);
		attr.c_cflag = attr.c_cflag | CLOCAL | CREAD;
		attr.c_cflag = (attr.c_cflag & static_cast<tcflag_t>(~CSIZE)) | CS8;

		if (tcsetattr(fd_, TCSANOW, &attr) == -1)
		{
			perror("serial_controller: tcsetattr");
			return false;
		}

		return true;
	}

	auto serial_controller::close() -> void
	{
		if (fd_ != -1)
		{
			::close(fd_);
			fd_ = -1;
		}
	}
} // namespace nes::app::mac
