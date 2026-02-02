//
//                  Simu5G
//
// Relay app: receives AlertPacket and forwards it.
//

package simu5g.apps.alert;

import inet.applications.contract.IApp;

simple AlertRelay like IApp
{
    parameters:
        int localPort = default(3000);

        // where to forward
        int destPort = default(3000);
        string destAddress;

        // loop prevention
        bool enableLoopPrevention = default(true);
        int maxSeen = default(500);

        // inet socket / multicast
        string interfaceTableModule;
        string multicastInterface = default("cellular"); // if not empty, set multicast output interface option (interface name expected)
        int tos = default(-1);

        @signal[alertDelay];
        @statistic[alertDelay](title="Alert Delay (as observed by Relay)"; unit="s"; source="alertDelay"; record=mean,vector);

        @signal[alertRcvdMsg];
        @statistic[alertRcvdMsg](title="Alert Messages Received by Relay"; unit=""; source="alertRcvdMsg"; record=sum,vector);

        @signal[alertFwdMsg];
        @statistic[alertFwdMsg](title="Alert Messages Forwarded by Relay"; unit=""; source="alertFwdMsg"; record=sum,vector);

        @display("i=block/blackboard");
    gates:
        output socketOut;
        input socketIn;
}
