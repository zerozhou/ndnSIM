#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
using namespace ns3;
int main(int argc, char* argv[])
{
    Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
    Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
    Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

    CommandLine cmd;
    cmd.Parse(argc, argv);
    
    NodeContainer nodes;
    nodes.Create(3);

    //wifi
    NodeContainer wifiNodes;
    wifiNodes.Create(1);
    NodeContainer wifiApNode=nodes.Get(0);


    PointToPointHelper p2p;
    p2p.Install(nodes.Get(0), nodes.Get(1));
    p2p.Install(nodes.Get(1), nodes.Get(2));

    
    YansWifiChannelHelper channel=YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy= YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    WifiHelper wifi = WifiHelper::Default();
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");
    
    NqosWifiMacHelper mac = NqosWifiMacHelper::Default();

    Ssid ssid = Ssid("ns-3-ssid");
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices;
    staDevices=wifi.Install(phy, mac, wifiNodes);
    
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy,mac,wifiApNode);

    //mobility module
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator", "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0), "DeltaX", DoubleValue(10.0),
                                 "DeltaY", DoubleValue(20.0), "GridWidth", UintegerValue(10),
                                 "LayoutType", StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue(Rectangle(-500,500,-500,500)));
    mobility.Install(wifiNodes);

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);


    ndn::StackHelper ndnHelper;
    ndnHelper.SetDefaultRoutes(true);
    ndnHelper.InstallAll();
    
    ndn::AppHelper consumerHelper("ns3::ndn::ConsumerCbr");
    consumerHelper.SetPrefix("/prefix");
    consumerHelper.SetAttribute("Frequency", StringValue("10"));
    consumerHelper.Install (wifiNodes.Get(0));

    ndn::AppHelper producerHelper("ns3::ndn::Producer");
    producerHelper.SetPrefix("/prefix");
    producerHelper.SetAttribute("PayloadSize", StringValue("1024"));
    producerHelper.Install(nodes.Get(2));

    Simulator::Stop(Seconds(20.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;


}
