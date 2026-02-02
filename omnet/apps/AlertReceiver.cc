//
//                  Simu5G
//
// Authors: Giovanni Nardini, Giovanni Stea, Antonio Virdis (University of Pisa)
//
// This file is part of a software released under the license included in file
// "license.pdf". Please read LICENSE and README files before using it.
// The above files and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "apps/alert/AlertReceiver.h"
#include <inet/common/ModuleAccess.h>  // for multicast support
#include "veins_inet/VeinsInetMobility.h"
#include "inet/mobility/contract/IMobility.h"

Define_Module(AlertReceiver);
using namespace inet;

void AlertReceiver::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;

    int port = par("localPort");
    EV << "AlertReceiver::initialize - binding to port: local:" << port << endl;
    if (port != -1)
    {
        socket.setOutputGate(gate("socketOut"));
        socket.bind(port);

        // for multicast support
        inet::IInterfaceTable *ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
        inet::MulticastGroupList mgl = ift->collectMulticastGroups();
        socket.joinLocalMulticastGroups(mgl);

        // if the multicastInterface parameter is not empty, set the interface explicitly
        const char *multicastInterface = par("multicastInterface");
        if (multicastInterface[0]) {
            NetworkInterface *ie = ift->findInterfaceByName(multicastInterface);
            if (!ie)
                throw cRuntimeError("Wrong multicastInterface setting: no interface named \"%s\"", multicastInterface);
            socket.setMulticastOutputInterface(ie->getInterfaceId());
        }

        // -------------------- //
    }

    alertDelay_ = registerSignal("alertDelay");
    alertRcvdMsg_ = registerSignal("alertRcvdMsg");
    sapaFrenadoSignal_ = registerSignal("sapaEmergencyStop");

    nrReceived = 0;
    delaySum = 0;
}

void AlertReceiver::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
        return;

    Packet* pPacket = check_and_cast<Packet*>(msg);

    // read Alert header
    auto alert = pPacket->popAtFront<AlertPacket>();

    // emit statistics
    simtime_t delay = simTime() - alert->getPayloadTimestamp();
    emit(alertDelay_, delay);
    emit(alertRcvdMsg_, (long)1);
    nrReceived++;
    delaySum+=delay;

    EV << "AlertReceiver::handleMessage - Packet received: SeqNo[" << alert->getSno() << "] Delay[" << delay << "]" << endl;

    cModule* host = getContainingNode(this);
    inet::IMobility* myMob = check_and_cast<inet::IMobility*>(host->getSubmodule("mobility"));
    veins::VeinsInetMobility* traciMob = dynamic_cast<veins::VeinsInetMobility*>(myMob);

    if (traciMob == nullptr) {
            delete msg;
            return;
        }

    inet::Coord myPos = myMob->getCurrentPosition();
    inet::Coord dangerPos(alert->getSenderPosX(), alert->getSenderPosY(), 0);
    double distance = myPos.distance(dangerPos);

    double myHeading = traciMob->getVehicleCommandInterface()->getAngle();
    double diffX = alert->getSenderPosX() - myPos.x;
    double diffY = alert->getSenderPosY() - myPos.y;

    EV << "DEBUG: myPos=(" << myPos.x << "," << myPos.y << ") "
       << "dangerPos=(" << dangerPos.x << "," << dangerPos.y << ") "
       << "distance=" << distance << "m heading=" << myHeading << endl;

    double h = myHeading * M_PI / 180.0;
    inet::Coord forward(std::sin(h), std::cos(h), 0);
    inet::Coord toAcc(diffX, diffY, 0);
    double dot = forward.x * toAcc.x + forward.y * toAcc.y;
    bool accidentInFront = (dot > 0);

    EV << "DEBUG2: forward=(" << forward.x << "," << forward.y << ") "
       << "toAcc=(" << toAcc.x << "," << toAcc.y << ") "
       << "dot=" << dot << " accidentInFront=" << accidentInFront << endl;


    const double stopDist = 60.0;
    const double warnDist = 100.0;


    if (distance <= stopDist && accidentInFront)
    {
        EV << "SAPA: ZONA CRITICA. Muy cerca y con accidente al frente. FRENADO TOTAL." << endl;
        traciMob->getVehicleCommandInterface()->setSpeed(0);
        traciMob->getVehicleCommandInterface()->setColor(veins::TraCIColor(255, 0, 0, 255));
    }

    else if (distance <= warnDist)
    {
        EV << "SAPA: ZONA PRECAUCION. Reduciendo velocidad. dist=" << distance << "m" << endl;
        traciMob->getVehicleCommandInterface()->setSpeed(3.0);
        traciMob->getVehicleCommandInterface()->setColor(veins::TraCIColor(255, 165, 0, 255));
    }

    else
    {
        EV << "SAPA: Fuera de rango. dist=" << distance << "m" << endl;
        traciMob->getVehicleCommandInterface()->setSpeed(-1); // devolver control a SUMO
        traciMob->getVehicleCommandInterface()->setColor(veins::TraCIColor(255, 255, 255, 255));
    }


    delete msg;
}


void AlertReceiver::refreshDisplay() const
{
    char buf[80];
    if(nrReceived >0){
        sprintf(buf, "received: %ld pks\nav. delay: %s s", nrReceived, (delaySum/nrReceived).format(-4).c_str());
    } else {
        sprintf(buf, "received: 0 pks");
    }
    getDisplayString().setTagArg("t", 0, buf);
}

