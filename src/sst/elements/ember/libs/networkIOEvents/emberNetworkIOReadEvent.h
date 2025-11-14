// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.

#pragma once

#include "emberNetworkIOEvent.h"

namespace SST {
namespace Ember {

class EmberNetworkIOReadEvent : public EmberNetworkIOEvent {
public:
    EmberNetworkIOReadEvent(NetworkIO::Interface& api, Output* output, Hermes::MemAddr dest,
                          uint64_t offset, uint32_t length, bool blocking,
                          EmberEventTimeStatistic* stat = NULL) :
        EmberNetworkIOEvent(api, output, stat),
        m_dest(dest), m_offset(offset), m_length(length), m_blocking(blocking)
    {}

    ~EmberNetworkIOReadEvent() {}

    std::string getName() { return "NetworkIORead"; }

    virtual void issue(uint64_t time, Callback callback) {
        EmberEvent::issue(time);
        m_api.networkIORead(m_dest.getSimVAddr(), m_offset, m_length, m_blocking, callback);
    }

private:
    Hermes::MemAddr m_dest;
    uint64_t m_offset;
    uint32_t m_length;
    bool m_blocking;
};

}
}
