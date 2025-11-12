#include "sst_config.h"

#include "simpleSSD.h"

using namespace SST;
using namespace Firefly;

SimpleSSD::SimpleSSD(ComponentId_t id, Params &params)
    : SimpleSSDAPI(id), m_pendingRequests(0) 
{
    m_pci.lanes.resize(params.find<int>("pcieLanesCount", 32));
    m_readOverheadLatency_ns = params.find<int64_t>("readOverheadLatency_ns", 500);
    m_writeOverheadLatency_ns = params.find<int64_t>("writeOverheadLatency_ns", 500);
    m_readBandwidthPerLane_GBps = params.find<double>("readBandwidthPerLane_GBps", 0.78125);
    m_writeBandwidthPerLane_GBps = params.find<double>("writeBandwidthPerLane_GBps", 0.78125);
    /// 0.78125 GB/s = 25(GBpS) divided by 32 lanes

    int verbosity = params.find<int>("verbosity", 0);
    m_out.init("[SimpleSSD] ", verbosity, 0, Output::STDOUT);
    registerClock("1GHz", new Clock::Handler2<SimpleSSD, &SimpleSSD::clockTick>(this));
    m_selfLink = configureSelfLink("ReadWriteLatency", "1 ns", new Event::Handler2<SimpleSSD, &SimpleSSD::handleEvent>(this));
}

void SimpleSSD::read(int64_t offset, size_t bytes, const SsdReqCallback &callback)
{
    enqueueRequest(offset, bytes, m_readBandwidthPerLane_GBps, m_readOverheadLatency_ns, callback);
}

void SimpleSSD::write(int64_t offset, size_t bytes, const SsdReqCallback &callback)
{
    enqueueRequest(offset, bytes, m_writeBandwidthPerLane_GBps, m_writeOverheadLatency_ns, callback);
}

void SimpleSSD::handleEvent(SST::Event *ev)
{
    DelayEvent *event = dynamic_cast<DelayEvent *>(ev);
    assert(event);
    event->m_callback();
    delete ev;
    --m_pendingRequests;
}

bool SimpleSSD::clockTick(SST::Cycle_t n)
{
    if (m_pendingRequests == 0)
    {
        for (int i = 0; i < m_pci.lanes.size(); ++i)
        {
            if (!m_pci.lanes.at(i).empty())
            {
                Request request = m_pci.lanes.at(i).front();
                DelayEvent *ev = new DelayEvent(request.callback);
                m_selfLink->send(request.delay_ns, ev);
                m_pci.lanes.at(i).pop();
                ++m_pendingRequests;
            }
        }
    }
    return false;
}

int64_t SimpleSSD::calcDelay_ns(const size_t bytes, const double bandwidth_GBps, const int64_t latency_ns)
{
    double delay_ns = latency_ns + ((bytes / bandwidth_GBps));
    return int64_t(delay_ns);
}

void SimpleSSD::enqueueRequest(const int64_t offset, const size_t bytes, const double bandwidth_GBps, const int64_t overheadLatency_ns, const SsdReqCallback& callback)
{
    Request request;
    request.bytes = bytes;
    request.delay_ns = this->calcDelay_ns(bytes, bandwidth_GBps, overheadLatency_ns);
    request.offset = offset;
    request.callback = callback;
    m_pci.currentLane %= m_pci.lanes.size();
    m_pci.lanes.at(m_pci.currentLane).push(request);
    m_pci.currentLane++;
}
