#include "../includes/ConfigParser.hpp"
#include "../includes/Kqueue.hpp"
#include "../includes/utils.hpp"
#include <sys/event.h> /* kqueue, kevent */
#include <iostream> /* std::cout */

#define ERROR -1

void	eventLoop( ConfigParser &conf ) {

	Kqueue			kq(&conf);
	struct kevent	trigger;
	int				newEvents = 0;

	while (true) {

		newEvents = kevent(kq.getFd(), NULL, 0, &trigger, 1, NULL);
		if (newEvents == ERROR)
			throw(Kqueue::KeventPollingError());

			if (kq.timeout())
				continue ;
			if (trigger.flags == EV_ERROR)
				kq.evError(trigger.ident);
			else if (trigger.flags & EV_EOF)
				kq.evEof(trigger.ident);
			else if (kq.locateSocket(trigger.ident) >= 0)
				kq.acceptConnection(trigger.ident);
			else if (trigger.filter == EVFILT_READ)
				kq.evRead(trigger.ident);
            else if (trigger.filter == EVFILT_WRITE)
				kq.evWrite(trigger.ident);
	}
}

int main ( int argc, char **argv ) {

	try {

		setSignals();
		ConfigParser conf(argc, argv);
		eventLoop(conf);
	}
	catch ( std::exception &e ) {

		std::cout << RED << e.what() << RESET << std::endl;
		return (1);
	}
	catch ( std::string e ) {

		std::cout << RED << e << RESET << std::endl;
		return (1);
	}
	return (0);
}
