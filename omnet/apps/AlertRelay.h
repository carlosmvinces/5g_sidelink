//
//                  Simu5G
//
// Relay app: receives AlertPacket and forwards it (repeater/RSU).
//

#ifndef _LTE_ALERTRELAY_H_
#define _LTE_ALERTRELAY_H_

#include <omnetpp.h>
#include <inet/transportlayer/contract/udp/UdpSocket.h>
#include <inet/networklayer/common/L3Address.h>
#include <inet/networklayer/common/L3AddressResolver.h>

#include <deque>
#include <unordered_set>
#include <string>

#include "apps/alert/AlertPacket_m.h"

class AlertRelay : public omnetpp::cSimpleModule
{
  protected:
    inet::UdpSocket socket;

    // RX/TX params
    int localPort_ = -1;
    int destPort_ = 3000;
    inet::L3Address destAddress_;

    // Loop prevention (seen cache)
    bool enableLoopPrevention_ = true;
    int maxSeen_ = 500; // keep last N seen alerts
    std::unordered_set<std::string> seen_;
    std::deque<std::string> seenOrder_;

    // Signals
    omnetpp::simsignal_t alertDelay_;
    omnetpp::simsignal_t alertRcvdMsg_;
    omnetpp::simsignal_t alertFwdMsg_;

  protected:
    virtual int numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void refreshDisplay() const override;

    std::string makeKey(const AlertPacket& a) const;
    bool markSeenIfNew(const std::string& key);

  public:
    AlertRelay() = default;
    virtual ~AlertRelay() = default;
};

#endif
