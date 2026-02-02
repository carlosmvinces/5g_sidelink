//
//                  Simu5G
//
// Relay app: receives AlertPacket and forwards it (repeater/RSU).
//

#include "apps/alert/AlertRelay.h"

#include <inet/common/ModuleAccess.h>   // multicast support
#include <inet/common/TimeTag_m.h>
#include <inet/networklayer/common/NetworkInterface.h>

#include <cmath>
#include <sstream>

Define_Module(AlertRelay);
using namespace inet;

void AlertRelay::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    localPort_ = par("localPort");
    destPort_  = par("destPort");
    destAddress_ = inet::L3AddressResolver().resolve(par("destAddress").stringValue());

    enableLoopPrevention_ = par("enableLoopPrevention");
    maxSeen_ = par("maxSeen");

    socket.setOutputGate(gate("socketOut"));

    if (localPort_ != -1) {
        socket.bind(localPort_);
    }

    int tos = par("tos");
    if (tos != -1)
        socket.setTos(tos);

    // multicast support (same pattern as your Sender/Receiver)
    inet::IInterfaceTable *ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
    inet::MulticastGroupList mgl = ift->collectMulticastGroups();
    socket.joinLocalMulticastGroups(mgl);

    const char *multicastInterface = par("multicastInterface");
    if (multicastInterface[0]) {
        NetworkInterface *ie = ift->findInterfaceByName(multicastInterface);
        if (!ie)
            throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
        socket.setMulticastOutputInterface(ie->getInterfaceId());
    }

    alertDelay_   = registerSignal("alertDelay");
    alertRcvdMsg_ = registerSignal("alertRcvdMsg");
    alertFwdMsg_  = registerSignal("alertFwdMsg");

    EV << "AlertRelay::initialize - localPort=" << localPort_
       << " dest=" << destAddress_ << ":" << destPort_
       << " loopPrevention=" << (enableLoopPrevention_ ? "true" : "false")
       << " maxSeen=" << maxSeen_
       << endl;
}

std::string AlertRelay::makeKey(const AlertPacket& a) const
{
    // Key uses: sno + sender position (rounded) to identify uniqueness
    // (no changes to AlertPacket.msg needed)
    const int xq = (int)std::lround(a.getSenderPosX());
    const int yq = (int)std::lround(a.getSenderPosY());

    std::ostringstream oss;
    oss << a.getSno() << "|" << xq << "|" << yq;
    return oss.str();
}

bool AlertRelay::markSeenIfNew(const std::string& key)
{
    if (!enableLoopPrevention_)
        return true; // always forward

    auto it = seen_.find(key);
    if (it != seen_.end())
        return false;

    // insert new
    seen_.insert(key);
    seenOrder_.push_back(key);

    // evict oldest if needed
    while ((int)seenOrder_.size() > maxSeen_) {
        const std::string& oldKey = seenOrder_.front();
        seen_.erase(oldKey);
        seenOrder_.pop_front();
    }
    return true;
}

void AlertRelay::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        delete msg;
        return;
    }

    Packet* rxPkt = check_and_cast<Packet*>(msg);
    auto originalAlert = rxPkt->peekAtFront<AlertPacket>();

    if (originalAlert) {
        simtime_t delay = simTime() - originalAlert->getPayloadTimestamp();
        emit(alertDelay_, delay);
        emit(alertRcvdMsg_, (long)1);

        const std::string key = makeKey(*originalAlert);
        const bool shouldForward = markSeenIfNew(key);

        EV << "AlertRelay: RX sno=" << originalAlert->getSno()
        << " forward=" << (shouldForward ? "YES" : "NO") << endl;

        if (shouldForward) {
            Packet* fwd = new Packet("AlertRelayed");
            auto relayedAlert = makeShared<AlertPacket>();
            relayedAlert->setSenderPosX(originalAlert->getSenderPosX());
            relayedAlert->setSenderPosY(originalAlert->getSenderPosY());
            relayedAlert->setPayloadTimestamp(originalAlert->getPayloadTimestamp());
            relayedAlert->setSno(originalAlert->getSno());
            relayedAlert->setChunkLength(originalAlert->getChunkLength());
            fwd->insertAtBack(relayedAlert);
            socket.sendTo(fwd, destAddress_, destPort_);
            emit(alertFwdMsg_, (long)1);
        }
    }

    delete msg;
}
void AlertRelay::refreshDisplay() const
{
    char buf[120];
    sprintf(buf, "relay\nseen=%zu", seen_.size());
    getDisplayString().setTagArg("t", 0, buf);
}
