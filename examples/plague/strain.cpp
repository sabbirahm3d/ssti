#include <sst/core/component.h>
#include <sst/core/interfaces/stringEvent.h>
#include <sst/core/link.h>

#include <iomanip>

#define SIMTIME 20
#define LOOPBEGIN 2
#define LOOPEND (SIMTIME - 2)
#define POPULATION_TOTAL 7760000000

class strain : public SST::Component {

public:

    strain(SST::ComponentId_t, SST::Params &);

    void setup() override;

    void finish() override;

    bool tick(SST::Cycle_t);

    void handle_event_pthresh_sc_ceil(SST::Event *);

    void handle_event_ram(SST::Event *);

    void handle_event_limit(SST::Event *);

    void handle_event_severity(SST::Event *);

    void handle_event_infectivity(SST::Event *);

    void handle_event_lethality(SST::Event *);

    void handle_event_minf_lethality(SST::Event *);

    void handle_event_minf_infectivity(SST::Event *);

    void handle_event_birth_rate(SST::Event *);

    SST_ELI_REGISTER_COMPONENT(
            strain,
            "plague",
            "strain",
            SST_ELI_ELEMENT_VERSION(1, 0, 0),
            "Traffic light simulator for the plague",
            COMPONENT_CATEGORY_UNCATEGORIZED
    )

    SST_ELI_DOCUMENT_PARAMS(
    )

    // Port name, description, event type
    SST_ELI_DOCUMENT_PORTS(
            { "limit_din", "limit_din", { "sst.Interfaces.StringEvent" }},
            { "limit_dout", "limit_dout", { "sst.Interfaces.StringEvent" }},
            { "severity_din", "severity_din", { "sst.Interfaces.StringEvent" }},
            { "severity_dout", "severity_dout", { "sst.Interfaces.StringEvent" }},
            { "infectivity_din", "infectivity_din", { "sst.Interfaces.StringEvent" }},
            { "infectivity_dout", "infectivity_dout", { "sst.Interfaces.StringEvent" }},
            { "lethality_din", "lethality_din", { "sst.Interfaces.StringEvent" }},
            { "lethality_dout", "lethality_dout", { "sst.Interfaces.StringEvent" }},
            { "minf_lethality_din", "minf_lethality_din", { "sst.Interfaces.StringEvent" }},
            { "minf_lethality_dout", "minf_lethality_dout", { "sst.Interfaces.StringEvent" }},
            { "minf_infectivity_din", "minf_infectivity_din", { "sst.Interfaces.StringEvent" }},
            { "minf_infectivity_dout", "minf_infectivity_dout", { "sst.Interfaces.StringEvent" }},
            { "birth_rate_din", "birth_rate_din", { "sst.Interfaces.StringEvent" }},
            { "birth_rate_dout", "birth_rate_dout", { "sst.Interfaces.StringEvent" }},
            { "pthresh_sc_ceil_din", "pthresh_sc_ceil_din", { "sst.Interfaces.StringEvent" }},
            { "pthresh_sc_ceil_dout", "pthresh_sc_ceil_dout", { "sst.Interfaces.StringEvent" }},
            { "ram_din", "ram_din", { "sst.Interfaces.StringEvent" }},
            { "ram_dout", "ram_dout", { "sst.Interfaces.StringEvent" }},
    )

private:

    // SST parameters
    std::string m_clock;
    int64_t m_seed0, m_seed1, m_seed2, m_seed3, m_seed4,
        m_seed5, m_seed6, m_seed7, m_seed8;

    // SST links and variables
    SST::Output m_output;
    SST::Link *ram_din_link, *ram_dout_link,
            *limit_din_link, *limit_dout_link,
            *infectivity_din_link, *infectivity_dout_link,
            *lethality_din_link, *lethality_dout_link,
            *severity_din_link, *severity_dout_link,
            *birth_rate_din_link, *birth_rate_dout_link,
            *pthresh_sc_ceil_din_link, *pthresh_sc_ceil_dout_link,
            *minf_lethality_din_link, *minf_lethality_dout_link,
            *minf_infectivity_din_link, *minf_infectivity_dout_link;

    bool keep_send, keep_recv;

    float SEVERITY, INFECTIVITY, LETHALITY, BIRTH_RATE;
    float m_lethality, m_infectivity, m_severity;
    int LIMIT, CURE_THRESHOLD;

    unsigned int m_cycle{};

};

strain::strain(SST::ComponentId_t id, SST::Params &params) :
        SST::Component(id),
        // Collect all the parameters from the project driver
        m_clock(params.find<std::string>("CLOCK", "1Hz")),
        m_seed0(params.find<int64_t>("SEED0", 12345)),
        m_seed1(params.find<int64_t>("SEED1", 12346)),
        m_seed2(params.find<int64_t>("SEED2", 12347)),
        m_seed3(params.find<int64_t>("SEED3", 12348)),
        m_seed4(params.find<int64_t>("SEED4", 12349)),
        m_seed5(params.find<int64_t>("SEED5", 12350)),
        m_seed6(params.find<int64_t>("SEED6", 12351)),
        m_seed7(params.find<int64_t>("SEED7", 12352)),
        m_seed8(params.find<int64_t>("SEED8", 12353)),
        ram_din_link(configureLink("ram_din")),
        ram_dout_link(configureLink(
                "ram_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_ram))),
        limit_din_link(configureLink("limit_din")),
        limit_dout_link(configureLink(
                "limit_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_limit))),
        infectivity_din_link(configureLink("infectivity_din")),
        infectivity_dout_link(configureLink(
                "infectivity_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_infectivity))),
        lethality_din_link(configureLink("lethality_din")),
        lethality_dout_link(configureLink(
                "lethality_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_lethality))),
        severity_din_link(configureLink("severity_din")),
        severity_dout_link(configureLink(
                "severity_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_severity))),
        birth_rate_din_link(configureLink("birth_rate_din")),
        birth_rate_dout_link(configureLink(
                "birth_rate_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_birth_rate))),
        pthresh_sc_ceil_din_link(configureLink("pthresh_sc_ceil_din")),
        pthresh_sc_ceil_dout_link(configureLink(
                "pthresh_sc_ceil_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_pthresh_sc_ceil))),
        minf_lethality_din_link(configureLink("minf_lethality_din")),
        minf_lethality_dout_link(configureLink(
                "minf_lethality_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_minf_lethality))),
        minf_infectivity_din_link(configureLink("minf_infectivity_din")),
        minf_infectivity_dout_link(configureLink(
                "minf_infectivity_dout",
                new SST::Event::Handler<strain>(this, &strain::handle_event_minf_infectivity)))
        {

    m_output.init("\033[93mstrain-" + getName() + "\033[0m -> ", 1, 0, SST::Output::STDOUT);

    // Just register a plain clock for this simple example
    registerClock(m_clock, new SST::Clock::Handler<strain>(this, &strain::tick));

}

void strain::setup() {

    m_output.verbose(CALL_INFO, 1, 0, "Component is being set up.\n");

}

void strain::finish() {

    m_output.verbose(CALL_INFO, 1, 0, "Destroying %s...\n", getName().c_str());

}

void strain::handle_event_ram(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        // if (m_cycle == 1) {
            // std::cout << "RAM " << m_cycle << ' ' << se->getString() << '\n';
        // }

    }

    delete se;

}

void strain::handle_event_pthresh_sc_ceil(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        // if (m_cycle == 1) {
        std::cout << "CEIL " << m_cycle << ' ' << se->getString() << '\n';
        // }

    }

    delete se;

}

void strain::handle_event_limit(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se) {

        if (m_cycle == 1) {
            std::cout << "LIM " << m_cycle << ' ' << se->getString() << '\n';
            LIMIT = std::stoi(se->getString());
        }

    }

    delete se;

}

void strain::handle_event_severity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "SEV " << se->getString() << '\n';
        SEVERITY = std::stof(se->getString());
    }

    delete se;

}

void strain::handle_event_birth_rate(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << "BIR " << se->getString() << '\n';
        BIRTH_RATE = std::stof(se->getString());
    }

    delete se;

}

void strain::handle_event_infectivity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        if (m_cycle == LOOPBEGIN) {
            INFECTIVITY = std::stof(se->getString());
        } else {

            std::cout << "INF " << m_cycle << ' ' << se->getString() << '\n';
            std::ostringstream _data_out;
            _data_out << std::fixed << std::setprecision(10) << (INFECTIVITY + std::stof(se->getString()));
            minf_infectivity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    _data_out.str() +
                    "0.99"
            ));

        }

    }

    delete se;

}

void strain::handle_event_lethality(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        if (m_cycle == LOOPBEGIN) {
            LETHALITY = std::stof(se->getString());
        } else {

            std::cout << "LET " << m_cycle << ' ' << se->getString() << '\n';
            std::ostringstream _data_out;
            _data_out << std::fixed << std::setprecision(10) << (LETHALITY + std::stof(se->getString()));
            minf_lethality_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    _data_out.str() +
                    "0.99"
            ));

        }

    }

    delete se;

}

void strain::handle_event_minf_lethality(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << "MIN LET " << se->getString() << '\n';
        LETHALITY = std::stof(se->getString());

    }

    delete se;

}

void strain::handle_event_minf_infectivity(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << "MIN INF " << se->getString() << '\n';
        INFECTIVITY = std::stof(se->getString());

    }

    delete se;

}

bool strain::tick(SST::Cycle_t current_cycle) {

    keep_send = current_cycle < SIMTIME;
    keep_recv = current_cycle < SIMTIME - 1;
    m_cycle = current_cycle;

    std::string m_data, dont_care = "00000000000000";

    ram_din_link->send(new SST::Interfaces::StringEvent(
            std::to_string(keep_send) +
            std::to_string(keep_recv) +
            "1234567811100000001"
    ));

    if (current_cycle == 1) {

        limit_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                std::to_string(m_seed0) +
                "0031000" +
                std::to_string(current_cycle)
        ));

    }

    if (current_cycle > 1) {

        if (current_cycle == 2) {

            severity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "1" +
                    std::to_string(m_seed1) +
                    "0020" +
                    std::to_string(LIMIT) +
                    std::to_string(current_cycle)
            ));

            birth_rate_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "1" +
                    std::to_string(m_seed2) +
                    "0020" +
                    std::to_string(LIMIT) +
                    std::to_string(current_cycle)
            ));

        } else if (current_cycle == 3) {

            std::cout << "LOL " << SEVERITY * LETHALITY * POPULATION_TOTAL << '\n';
            pthresh_sc_ceil_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    std::to_string(SEVERITY * LETHALITY * POPULATION_TOTAL)
            ));

        } else if (!keep_recv) {

            severity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    std::to_string(m_seed1) +
                    "0020" +
                    std::to_string(current_cycle)
            ));

            birth_rate_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    std::to_string(m_seed2) +
                    "0020" +
                    std::to_string(current_cycle)
            ));

            limit_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    std::to_string(m_seed0) +
                    "0031000" +
                    std::to_string(current_cycle)
            ));

            lethality_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    std::to_string(m_seed3) +
                    "0020" +
                    std::to_string(LIMIT) +
                    std::to_string(current_cycle)
            ));

            infectivity_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(keep_send) +
                    std::to_string(keep_recv) +
                    "0" +
                    std::to_string(m_seed4) +
                    "0020" +
                    std::to_string(LIMIT) +
                    std::to_string(current_cycle)
            ));

        }


        lethality_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                std::to_string(m_seed3) +
                "0020" +
                std::to_string(LIMIT) +
                std::to_string(current_cycle)
        ));

        infectivity_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                std::to_string(m_seed4) +
                "0020" +
                std::to_string(LIMIT) +
                std::to_string(current_cycle)
        ));

    }

    return false;

}
