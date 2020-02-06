#ifndef PLAGUE_EVENT_DRIVERS_HPP
#define PLAGUE_EVENT_DRIVERS_HPP

#include "plague.hpp"
#include "setup.hpp"

#include <iomanip>

void plague::ram(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << m_cycle << " RAM " << se->getString() << '\n';

    }

    delete se;

}

void plague::rng_limit(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << m_cycle << " LIM " << se->getString() << '\n';
        LIMIT = se->getString();
        align_signal_width('0', 4, LIMIT);

        randf_sev_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_sev +
                "002" +
                LIMIT +
                std::to_string(m_cycle)
        ));

        randf_br_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                "1" +
                seed_birth_rate +
                "002" +
                LIMIT +
                std::to_string(m_cycle)
        ));

    }

    delete se;

}

void plague::randf_sev(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << m_cycle << " SEV " << se->getString() << '\n';
        SEVERITY = std::stof(se->getString());
    }

    delete se;

}

void plague::randf_br(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && keep_recv) {

        std::cout << m_cycle << " BIR " << se->getString() << '\n';
        BIRTH_RATE = std::stof(se->getString());
    }

    delete se;

}

void plague::ceil_cure_thresh(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && keep_recv) {

        std::cout << m_cycle << " THRESH (CEIL) " << se->getString() << '\n';
        CURE_THRESHOLD = se->getString();
        align_signal_width('0', 8, CURE_THRESHOLD);

        std::string addr = std::to_string(ADDRESSES::CURE_THRESHOLD_ADDR);
        align_signal_width('0', 8, addr);

        ram_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(keep_send) +
                std::to_string(keep_recv) +
                addr +
                m_ram_write +
                CURE_THRESHOLD
        ));

    }

    delete se;

}

void plague::randf_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        std::cout << m_cycle << " LET (RANDF) " << se->getString() << '\n';
        if (m_cycle == LOOPBEGIN) {

            LETHALITY = std::stof(se->getString());

        } else {

            min_let_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, LETHALITY + std::stof(se->getString())) +
                    "0.99"
            ));

        }

    }

    delete se;

}

void plague::min_let(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << m_cycle << " LET (MIN) " << se->getString() << '\n';
        LETHALITY = std::stof(se->getString());

    }

    delete se;

}

void plague::randf_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 1;
    bool _keep_recv = m_cycle < SIMTIME - 2;

    if (se && keep_recv) {

        std::cout << m_cycle << " INF (RANDF) " << se->getString() << '\n';
        if (m_cycle == LOOPBEGIN) {

            INFECTIVITY = std::stof(se->getString());

        } else {

            min_inf_din_link->send(new SST::Interfaces::StringEvent(
                    std::to_string(_keep_send) +
                    std::to_string(_keep_recv) +
                    align_signal_width(10, INFECTIVITY + std::stof(se->getString())) +
                    "0.99"
            ));

        }

    }

    delete se;

}

void plague::min_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 2;
    bool _keep_recv = m_cycle < SIMTIME - 3;

    if (se && m_cycle < LOOPEND) {

        std::cout << m_cycle << " INF (MIN) " << se->getString() << '\n';
        INFECTIVITY = std::stof(se->getString());
        exp_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(10, INFECTIVITY)
        ));

    }

    delete se;

}

void plague::exp_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 3;
    bool _keep_recv = m_cycle < SIMTIME - 4;

    if (se && m_cycle < LOOPEND - 1) {

        std::cout << m_cycle << " INF (EXP) " << se->getString() << '\n';
        ceil_pop_inf_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(2, std::stof(se->getString()) * BATCH_INFECTED)
        ));

    }

    delete se;

}

void plague::rng_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    if (se && m_cycle < LOOPEND) {

        std::cout << m_cycle << " POP INF (RANDF) " << se->getString() << '\n';
        BATCH_INFECTED = std::stoi(se->getString());

    }

    delete se;

}

void plague::ceil_pop_inf(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);
    bool _keep_send = m_cycle < SIMTIME - 4;
    bool _keep_recv = m_cycle < SIMTIME - 5;

    if (se && m_cycle < LOOPEND - 2) {

        std::cout << m_cycle << " POP INF (CEIL) " << se->getString() << '\n';
        ceil_pop_dead_din_link->send(new SST::Interfaces::StringEvent(
                std::to_string(_keep_send) +
                std::to_string(_keep_recv) +
                align_signal_width(2, std::stoi(se->getString()) * LETHALITY)
        ));

    }

    delete se;

}

void plague::ceil_pop_dead(SST::Event *ev) {

    auto *se = dynamic_cast<SST::Interfaces::StringEvent *>(ev);

    if (se && m_cycle < LOOPEND - 3) {

        std::cout << m_cycle << " POP DEAD (CEIL) " << se->getString() << '\n';

    }

    delete se;

}

#endif