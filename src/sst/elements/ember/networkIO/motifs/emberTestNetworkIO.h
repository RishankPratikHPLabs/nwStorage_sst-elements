// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.

#if 0

#pragma once

#include <strings.h>
#include <sst/core/rng/marsaglia.h>
#include "network/emberNetworkIOGen.h"

namespace SST {
namespace Ember {

class EmberTestNetworkIOGenerator : public EmberNetworkIOGenerator {
public:
    SST_ELI_REGISTER_SUBCOMPONENT(
        EmberTestNetworkIOGenerator,
        "ember",
        "TestNetworkIOMotif",
        SST_ELI_ELEMENT_VERSION(1,0,0),
        "Network IO Test",
        SST::Ember::EmberGenerator
    )
    
    SST_ELI_DOCUMENT_PARAMS(
        {"arg.messageSize","Message size in bytes","1024"},
        {"arg.iterations","Number of iterations to perform","1"},
        {"arg.op","Operation type: read or write","write"},
        {"arg.fileSize","Storage file size in bytes","10485760"},
        {"arg.blocking","Use blocking operations","true"}
    )

public:
    EmberTestNetworkIOGenerator(SST::ComponentId_t id, Params& params) :
        EmberNetworkIOGenerator(id, params, "TestNetworkIO"),
        m_phase(0)
    {
        m_messageSize = params.find<uint32_t>("arg.messageSize", 1024);
        m_iterations = params.find<uint32_t>("arg.iterations", 1);
        m_opType = params.find<std::string>("arg.op", "write");
        m_fileSize = params.find<uint64_t>("arg.fileSize", 10485760);  // 10MB default
        m_blocking = params.find<bool>("arg.blocking", true);
        
        m_rngSeed = 100;
        m_rng = new SST::RNG::MarsagliaRNG();
        m_storageOffset = 0;
        m_startTime = 0;
        m_stopTime = 0;
        m_my_pe = -1;
    }

    bool generate( std::queue<EmberEvent*>& evQ) override
    {
        bool ret = false;
        
        switch(m_phase) {
            case 0:
                // Phase 0: Initialize SHMEM, get PE info, allocate buffer
                // Note: Network storage is auto-initialized in HadesNetwork
                shmem().init(evQ);
                enQ_my_pe(evQ, &m_my_pe);
                enQ_malloc(evQ, &m_localBuffer, m_messageSize);
                break;
                
            case 1:
                // Phase 1: Seed RNG (unique per PE) and initialize data buffer
                m_rng->seed(m_rngSeed + m_my_pe);
                if (m_opType == "write") {
                    for (uint32_t i = 0; i < m_messageSize; i++) {
                        m_localBuffer.at<uint8_t>(i) = m_my_pe + i;
                    }
                } else {
                    bzero(&m_localBuffer.at<uint8_t>(0), m_messageSize);
                }
                enQ_barrier_all(evQ);
                break;
                
            case 2:
                // Phase 2: Perform all iterations - generate random offset and read/write
                enQ_getTime(evQ, &m_startTime);
                for (uint32_t i = 0; i < m_iterations; i++) {
                    m_storageOffset = m_rng->generateNextUInt64() % m_fileSize;
                    
                    if (m_opType == "read") {
                        network().network_read(evQ, m_localBuffer, m_storageOffset, m_messageSize, m_blocking);
                    } else {
                        network().network_write(evQ, m_storageOffset, m_localBuffer, m_messageSize, m_blocking);
                    }
                }
                enQ_getTime(evQ, &m_stopTime);
                break;
                
            case 3:
                // Phase 3: Print performance results (PE 0 only)
                if ( 0 == m_my_pe ) {
                    double totalTime = (double)(m_stopTime - m_startTime)/1000000000.0;
                    double latency = (totalTime/m_iterations);
                    output("message-size %u, iterations %u, total-time %.3lf us\n",
                           m_messageSize, m_iterations,
                           totalTime * 1000000.0, latency * 1000000.0);
                }
                ret = true;
                break;
        }
        
        ++m_phase;
        return ret;
    }

private:
    // Simulation control
    int m_phase;
    
    // SHMEM parameters
    int m_my_pe;
    
    // Random number generation
    int m_rngSeed;                   // Base seed value (unique per PE)
    SST::RNG::MarsagliaRNG* m_rng;   // RNG instance for offset generation
    
    // Operation parameters
    uint32_t m_messageSize;          // Size of each read/write operation
    uint32_t m_iterations;           // Number of operations to perform
    uint64_t m_fileSize;             // Total file size for offset generation
    bool m_blocking;                 // Blocking vs non-blocking operations
    std::string m_opType;            // Operation type: "read" or "write"
    
    // Runtime state
    uint64_t m_storageOffset;        // Current storage offset
    Hermes::MemAddr m_localBuffer;   // Buffer for data transfer
    
    // Performance measurement
    uint64_t m_startTime;
    uint64_t m_stopTime;
};

}
}

#endif
