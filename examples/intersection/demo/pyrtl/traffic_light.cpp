#include "../../blackboxes/traffic_light_fsm_ports.hpp"
#include "../../../../sstscit/sstscit.hpp"

#include <sst/core/sst_config.h>
#include <sst/core/component.h>
#include <sst/core/elementinfo.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#define SIMTIME 86400

class traffic_light_pyrtl : public SST::Component {

public:

    traffic_light_pyrtl(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    SST_ELI_REGISTER_COMPONENT(
        traffic_light_pyrtl,
        "intersection",
        "traffic_light_pyrtl",
        SST_ELI_ELEMENT_VERSION(1, 0, 0),
        "Traffic light simulator for the intersection",
        COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
        { "CLOCK", "Clock frequency or period", "1Hz" },
        { "GREENTIME", "Duration of the green light staying on", "30" },
        { "YELLOWTIME", "Duration of the yellow light staying on", "3" },
        { "REDTIME", "Duration of the red light staying on", "33" },
        { "STARTGREEN", "Flag to initiate the light as green or red", "0" },
        { "PROC", "Path to compiled SystemC driver", "/path/to/systemc/driver" },
        { "IPC_PORT", "Path to the IPC file", "/tmp/ABCDEF" },
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
        { "light_state", "Port on which to send/recv messages", { "sst.Interfaces.StringEvent" }}
    )

private:

    // Prepare the signal handler
    SocketSignal m_signal_io;

    // SST parameters
    std::string m_clock;
    int STARTGREEN, GREENTIME, YELLOWTIME, REDTIME;
    std::string m_proc, m_ipc_port;

    // SST links and variables
    SST::Output m_output;
    SST::Link *light_state;

};

traffic_light_pyrtl::traffic_light_pyrtl(SST::ComponentId_t id, SST::Params &params) :
    SST::Component(id),
    m_signal_io(TRFFCLGHTFSM_NPORTS, socket(AF_UNIX, SOCK_STREAM, 0)),
    // Collect all the parameters from the project driver
    m_clock(params.find<std::string>("CLOCK", "1Hz")),
    STARTGREEN(params.find<int>("STARTGREEN", false)),
    GREENTIME(params.find<int>("GREENTIME", 30)),
    YELLOWTIME(params.find<int>("YELLOWTIME", 3)),
    REDTIME(params.find<int>("REDTIME", 33)),
    m_proc(params.find<std::string>("PROC", "")),
    m_ipc_port(params.find<std::string>("IPC_PORT", "")),
    light_state(configureLink("light_state")) {

    m_output.init("\033[93mtraffic_light-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Check parameters
    if (!(GREENTIME && YELLOWTIME && REDTIME)) {
        m_output.fatal(CALL_INFO, -1, "Error: times must be greater than zero.\n");
    }

    // Print parameters
    m_output.verbose(CALL_INFO, 1, 0, "GREENTIME=%d, YELLOWTIME=%d, REDTIME=%d, STARTGREEN=%d\n",
                     GREENTIME, YELLOWTIME, REDTIME, STARTGREEN);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<traffic_light_pyrtl>(this, &traffic_light_pyrtl::tick));

    // Configure our ports
    if (!light_state) {
        m_output.fatal(CALL_INFO, -1, "Failed to configure light_state 'light_state'\n");
    }

}

void traffic_light_pyrtl::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

    int child_pid = fork();

    if (!child_pid) {

        char *args[] = {"python3", &m_proc[0u], &m_ipc_port[0u], nullptr};
        m_output.verbose(CALL_INFO, 1, 0, "Forking process \"%s\"...\n", m_proc.c_str());
        execvp(args[0], args);

    } else {

        m_signal_io.set_addr(m_ipc_port);
        m_signal_io.recv();
        if (child_pid == m_signal_io.get<int>(trffclghtfsm_ports.pid)) {
            m_output.verbose(CALL_INFO, 1, 0, "Process \"%s\" successfully synchronized\n",
                             m_proc.c_str());
        }

    }

}

void traffic_light_pyrtl::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

// Send a command to the PyRTL stopLight every clock
bool traffic_light_pyrtl::tick(SST::Cycle_t current_cycle) {

    bool keep_send = current_cycle < SIMTIME;
    bool keep_recv = current_cycle < SIMTIME - 1;

    bool load;
    int start_green, green_time, yellow_time, red_time;

    if (current_cycle == 1) {
        load = true;
        start_green = STARTGREEN;
        green_time = GREENTIME;
        yellow_time = YELLOWTIME;
        red_time = REDTIME;
    } else {
        load = false;
        start_green = 0;
        green_time = 0;
        yellow_time = 0;
        red_time = 0;
    }

    // outputs to SystemC child process
    m_signal_io.set(trffclghtfsm_ports.load, load);
    m_signal_io.set(trffclghtfsm_ports.start_green, start_green);
    m_signal_io.set(trffclghtfsm_ports.green_time, green_time);
    m_signal_io.set(trffclghtfsm_ports.yellow_time, yellow_time);
    m_signal_io.set(trffclghtfsm_ports.red_time, red_time);
    m_signal_io.set(trffclghtfsm_ports.clock, current_cycle);

    if (keep_send) {
        m_signal_io.set_state(keep_recv);
        m_signal_io.send();
    }
    if (keep_recv) {
        m_signal_io.recv();
    }

    switch (m_signal_io.get<int>(trffclghtfsm_ports.state)) {
        case 0:
            light_state->send(new SST::Interfaces::StringEvent("green"));
            break;
        case 1:
            light_state->send(new SST::Interfaces::StringEvent("yellow"));
            break;
        case 2:
            light_state->send(new SST::Interfaces::StringEvent("red"));
            break;
    }

    return false;

}


// // Send a command to the PyRTL stopLight every clock
// bool trafficLight::clockTick( SST::Cycle_t currentCycle ) {
// 	write(outFifo, s, 8);
// 	// Clear the command so we don't send the same command over and over
// 	// need to receive a command from the port
// 	strncpy(s, "00000000", 8);
	
// 	// Read from the PyRTL stopLight to see what state the light is in
// 	char r[2] = "\0";
// 	read(inFifo, r, 1);
// 	std::string c;
// 	switch(r[0]){
// 		case '0':
// 			c = "green";
// 			break;
// 		case '1':
// 			c = "yellow";
// 			break;
// 		case '2':
// 			c = "red";
// 			break;
// 	}
// 	port->send(new StringEvent(c));
// 	return false;
// }

// // Send commands that are received to the PyRTL stopLight
// void trafficLight::handleEvent(SST::Event *ev) {
// 	StringEvent *se = dynamic_cast<StringEvent*>(ev);
// 	if ( se != NULL ) {
// 		strncpy(s, se->getString().c_str(), 8);
// 	}
// 	delete ev;
// }