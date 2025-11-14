// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.

#pragma once

#include "sst/elements/ember/embergen.h"
#include "sst/elements/ember/libs/emberLib.h"
#include "sst/elements/ember/libs/networkIOEvents/emberNetworkIOReadEvent.h"
#include "sst/elements/ember/libs/networkIOEvents/emberNetworkIOWriteEvent.h"
#include "sst/elements/hermes/networkIOapi.h"

using namespace Hermes;

namespace SST {
namespace Ember {

class EmberNetworkIOLib : public EmberLib {
public:
    SST_ELI_REGISTER_MODULE(
        EmberNetworkIOLib,
        "ember",
        "networkIOLib",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Network I/O Library for network-attached storage operations",
        SST::Ember::EmberLib
    )
    
    SST_ELI_DOCUMENT_PARAMS()

    EmberNetworkIOLib(Params& params) {}

    void networkIORead(std::queue<EmberEvent*>& q, Hermes::MemAddr dest, uint64_t offset, uint64_t length, bool blocking = true)
    {
        q.push(new EmberNetworkIOReadEvent(api(), m_output, dest, offset, length, blocking));
    }

    void networkIOWrite(std::queue<EmberEvent*>& q, uint64_t offset, Hermes::MemAddr src, uint64_t length, bool blocking = true)
    {
        q.push(new EmberNetworkIOWriteEvent(api(), m_output, offset, src, length, blocking));
    }
    
private:
    NetworkIO::Interface& api() { return *static_cast<NetworkIO::Interface*>(m_api); }
};

}
}
