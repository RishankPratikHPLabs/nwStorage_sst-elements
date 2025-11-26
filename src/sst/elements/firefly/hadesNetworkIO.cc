// Copyright 2013-2025 NTESS.
// All rights reserved.

#include "sst_config.h"

#include "hades.h"
#include "hadesNetworkIO.h"

using namespace SST::Firefly;

HadesNetworkIO::HadesNetworkIO(ComponentId_t id, Params& params) :
    Hermes::NetworkIO::Interface(id),
    m_nicPtr(NULL)
{
    m_dbg.init("@t:HadesNetwork::@p():@l ",
        params.find<uint32_t>("verboseLevel",0),
        params.find<uint32_t>("verboseMask",-1),
        Output::STDOUT );
    
}

void HadesNetworkIO::setOS( Hermes::OS* os )
{
    m_osPtr = dynamic_cast<Hades*>(os);
    assert(m_osPtr);
    m_nicPtr = m_osPtr->getNic();
}

void HadesNetworkIO::setup()
{
}


void HadesNetworkIO::networkIORead(Hermes::Vaddr dest, uint64_t offset, uint64_t length,
                             bool isBlocking, Callback callback)
{
    m_dbg.verbose(CALL_INFO, 1, 0, "network_read: dest=%lx offset=%lu length=%lu blocking=%d\n", 
                  dest, offset, length, isBlocking);
    callback(0);
    //m_nic->networkRead(dest, offset, length, isBlocking, callback);
}

void HadesNetworkIO::networkIOWrite(uint64_t offset, Hermes::Vaddr src, uint64_t length,
                              bool isBlocking, Callback callback)
{
    m_dbg.verbose(CALL_INFO, 1, 0, "network_write: offset=%lu src=%lx length=%lu blocking=%d\n", 
                  offset, src, length, isBlocking);
    callback(0);
    //m_nic->networkWrite(offset, src, length, isBlocking, callback);
}
