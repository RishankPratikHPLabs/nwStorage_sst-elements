// Copyright 2009-2025 NTESS. Under the terms
// of Contract DE-NA0003525 with NTESS, the U.S.
// Government retains certain rights in this software.
//
// Copyright (c) 2009-2025, NTESS
// All rights reserved.

#pragma once
#include "sst/elements/ember/libs/emberNetworkIOLib.h"

using namespace Hermes;

namespace SST {
namespace Ember {

class EmberNetworkIOGenerator : public EmberGenerator {
public:
    EmberNetworkIOGenerator(ComponentId_t id, Params& params, std::string name = "");
    ~EmberNetworkIOGenerator() {}
    virtual void completed(const SST::Output*, uint64_t time) {}
    virtual void setup();

protected:
    EmberNetworkIOLib* m_networkIOLib;
    EmberNetworkIOLib& networkIO() { return *m_networkIOLib; }
};

}
}

