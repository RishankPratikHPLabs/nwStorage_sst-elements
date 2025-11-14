// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.

#include <sst_config.h>
#include "emberNetworkIOGen.h"

using namespace SST::Ember;

EmberNetworkIOGenerator::EmberNetworkIOGenerator(ComponentId_t id, Params& params, std::string name) 
    : EmberGenerator(id, params, name), m_networkIOLib(nullptr)
{
}

void EmberNetworkIOGenerator::setup()
{
    m_networkIOLib = static_cast<EmberNetworkIOLib*>(getLib("networkIO"));
    assert(m_networkIOLib);
}
