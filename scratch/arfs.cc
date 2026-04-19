#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/aodv-module.h"
#include "ns3/flow-monitor-module.h"

#include <iostream>
#include <vector>
#include <cmath>
#include<fstream>

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE ("ARFS_Algorithm");

int main ()
{
    int n;
    double delta, gamma;
    int CWmin, m;
    int nPackets;

    cout << "Enter number of nodes (n): ";
    cin >> n;

    cout << "Enter base fragment size (delta): ";
    cin >> delta;

    cout << "Enter base transmission rate (gamma): ";
    cin >> gamma;

    cout << "Enter CWmin: ";
    cin >> CWmin;

    cout << "Enter backoff stages (m): ";
    cin >> m;

    cout << "Enter number of packets: ";
    cin >> nPackets;

    // ===== NODE SETUP =====
    NodeContainer nodes;
    nodes.Create(n);

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);

    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
        "DataMode", StringValue("ErpOfdmRate6Mbps"),
        "ControlMode", StringValue("ErpOfdmRate6Mbps"));

    YansWifiPhyHelper phy;
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    mac.SetType("ns3::AdhocWifiMac");

    NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
        "MinX", DoubleValue(0.0),
        "DeltaX", DoubleValue(50.0),
        "GridWidth", UintegerValue(n));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    AodvHelper aodv;
    InternetStackHelper stack;
    stack.SetRoutingHelper(aodv);
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    auto interfaces = address.Assign(devices);

    // ================= ARFS ALGORITHM =================

    vector<double> h(n), k(n);
    vector<double> tau(n), p(n), phi(n);
    vector<double> W(n), Wnorm(n);
    vector<double> frag(n), rate(n);

    // Step 1: Define hops (h) and interfering nodes (k)
    for (int i = 0; i < n; i++)
    {
        h[i] = (n - 1) - i;   // far → large hops
        if (h[i] == 0) h[i] = 1;

        k[i] = 2 + (i % 3);  // simple assumption
    }

    // Step 2 & 3: Compute tau and p
    for (int i = 0; i < n; i++)
    {
        p[i] = 0.1; // initial guess

        // compute tau using equation
        tau[i] = (2 * (1 - 2 * p[i]) * (1 - p[i])) /
                 ((1 - 2 * p[i]) * (CWmin + 1) +
                  p[i] * CWmin * (1 - pow((2 * p[i]), m)));

        // update p using tau
        p[i] = 1 - pow((1 - tau[i]), k[i] - 1);
    }

    // Step 4: Compute phi
    for (int i = 0; i < n; i++)
    {
        phi[i] = 1 - p[i]; 
    }

    // Step 5: Compute weights
    double maxW = 0;
    for (int i = 0; i < n; i++)
    {
        W[i] = 1.0 / pow(phi[i], h[i]);

        if (W[i] > maxW)
            maxW = W[i];
    }

    // Step 6: Normalize weights
    for (int i = 0; i < n; i++)
    {
        Wnorm[i] =  maxW/W[i];
    }

    // Step 7 & 8: Compute fragment size and rate
    for (int i = 0; i < n; i++)
    {
        frag[i] = delta * Wnorm[i];
        rate[i] = gamma * Wnorm[i];
    }

    // ===== DISPLAY =====
    ofstream fout("arfs_output.txt");
    cout << "\n===== ARFS COMPUTATION TABLE =====\n";
    fout << "===== ARFS COMPUTATION TABLE =====\n";

    for (int i = 0; i < n; i++)
    {
       cout << "Node " << i
     << " | h=" << h[i]
     << " | k=" << k[i]
     << " | tau=" << tau[i]
     << " | p=" << p[i]
     << " | phi=" << phi[i]
     << " | W=" << W[i]
     << " | NormW=" << Wnorm[i]
     << " | Frag=" << frag[i]
     << " | Rate=" << rate[i]
     << endl;

    fout << "Node " << i
     << " | h=" << h[i]
     << " | k=" << k[i]
     << " | tau=" << tau[i]
     << " | p=" << p[i]
     << " | phi=" << phi[i]
     << " | W=" << W[i]
     << " | NormW=" << Wnorm[i]
     << " | Frag=" << frag[i]
     << " | Rate=" << rate[i]
     << endl;
    }
    cout<<endl;
    // ================= APPLICATION =================

    UdpServerHelper server(9);
    auto serverApp = server.Install(nodes.Get(n - 1));
    serverApp.Start(Seconds(1));
    serverApp.Stop(Seconds(10));

    UdpClientHelper client(interfaces.GetAddress(n - 1), 9);
    client.SetAttribute("MaxPackets", UintegerValue(nPackets));
    client.SetAttribute("Interval", TimeValue(MilliSeconds(10)));

    // use ARFS fragment (node 0)
    client.SetAttribute("PacketSize", UintegerValue((uint32_t)frag[0]));

    auto clientApp = client.Install(nodes.Get(0));
    clientApp.Start(Seconds(2));
    clientApp.Stop(Seconds(10));

    // ===== FLOW MONITOR =====
    FlowMonitorHelper fm;
    auto monitor = fm.InstallAll();

    Simulator::Stop(Seconds(10));
    Simulator::Run();

    double totalThroughput = 0;

    for (auto &flow : monitor->GetFlowStats())
    {
        double t = flow.second.rxBytes * 8.0 /
            (flow.second.timeLastRxPacket.GetSeconds() -
             flow.second.timeFirstTxPacket.GetSeconds());

        totalThroughput += t;
    }
    
    monitor->CheckForLostPackets();
    auto stats = monitor->GetFlowStats();

    for (auto const &flow : stats)
    {
     if (flow.second.txPackets <= 1) continue;

     std::cout << "Flow " << flow.first
          << " Tx=" << flow.second.txPackets
          << " Rx=" << flow.second.rxPackets
          << std::endl;

     fout << "Flow " << flow.first
          << " Tx=" << flow.second.txPackets
          << " Rx=" << flow.second.rxPackets
          << std::endl;
    }

    cout << "\nTOTAL THROUGHPUT: "
     << totalThroughput / 1024 / 1024
     << " Mbps\n";

    fout << "TOTAL THROUGHPUT: "
     << totalThroughput / 1024 / 1024
     << " Mbps\n";

    fout.close();

    Simulator::Destroy();
    return 0;
}