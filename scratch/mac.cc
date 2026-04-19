#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/aodv-module.h"
#include "ns3/flow-monitor-module.h"
#include <iostream>
#include<fstream>

using namespace ns3;
using namespace std;

int main ()
{
    int nNodes, packetSize, dataRate, nPackets;

    cout << "Enter number of nodes: ";
    cin >> nNodes;

    cout << "Enter packet size (bytes): ";
    cin >> packetSize;

    cout << "Enter transmission rate (6/24/54 Mbps): ";
    cin >> dataRate;

    cout << "Enter number of packets: ";
    cin >> nPackets;

    // ===== NODES =====
    NodeContainer nodes;
    nodes.Create(nNodes);

    // ===== WIFI =====
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);

    string mode;
    if (dataRate <= 6)
        mode = "ErpOfdmRate6Mbps";
    else if (dataRate <= 24)
        mode = "ErpOfdmRate24Mbps";
    else
        mode = "ErpOfdmRate54Mbps";

    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
        "DataMode", StringValue(mode),
        "ControlMode", StringValue(mode));

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    // Strong signal
    phy.Set("TxPowerStart", DoubleValue(20));
    phy.Set("TxPowerEnd", DoubleValue(20));

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    // ===== MOBILITY (FIXED RANGE) =====
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
        "MinX", DoubleValue(0.0),
        "DeltaX", DoubleValue(35.0),
        "GridWidth", UintegerValue(nNodes));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    // ===== INTERNET + AODV =====
    AodvHelper aodv;
    InternetStackHelper stack;
    stack.SetRoutingHelper(aodv);
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    auto interfaces = address.Assign(devices);

    // ===== APPLICATION =====
    UdpServerHelper server(9);
    auto serverApp = server.Install(nodes.Get(nNodes - 1));
    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(20));

    UdpClientHelper client(interfaces.GetAddress(nNodes - 1), 9);
    client.SetAttribute("MaxPackets", UintegerValue(nPackets));
    client.SetAttribute("Interval", TimeValue(MilliSeconds(5)));
    client.SetAttribute("PacketSize", UintegerValue(packetSize));

    auto clientApp = client.Install(nodes.Get(0));

    clientApp.Start(Seconds(7));
    clientApp.Stop(Seconds(20));

    // ===== FLOW MONITOR =====
    FlowMonitorHelper fm;
    Ptr<FlowMonitor> monitor = fm.InstallAll();

    Simulator::Stop(Seconds(20));
    Simulator::Run();

    // ===== RESULT =====
    double totalThroughput = 0;
    ofstream fout("mac_output.txt");
    fout << "===== NORMAL MAC RESULT =====\n";

    cout << "\n===== NORMAL MAC RESULT =====\n";

    for (auto &flow : monitor->GetFlowStats())
    {
        if (flow.second.rxBytes > 0)
        {
            double duration =
                flow.second.timeLastRxPacket.GetSeconds() -
                flow.second.timeFirstTxPacket.GetSeconds();

            if (duration > 0)
            {
                double t = flow.second.rxBytes * 8.0 / duration;
                totalThroughput += t;
            }
        }
        if(flow.second.txPackets <= 1) continue;
        cout << "Flow " << flow.first
     << " Tx=" << flow.second.txPackets
     << " Rx=" << flow.second.rxPackets << endl;

     fout << "Flow " << flow.first
     << " Tx=" << flow.second.txPackets
     << " Rx=" << flow.second.rxPackets << endl;
    }

    cout << "TOTAL THROUGHPUT: "
     << totalThroughput / 1024 / 1024
     << " Mbps\n";

    fout << "TOTAL THROUGHPUT: "
     << totalThroughput / 1024 / 1024
     << " Mbps\n";

    fout.close();

    Simulator::Destroy();
}