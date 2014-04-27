#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ndnSIM-module.h"
//#include "user-reader.h"
#include "test-app.h"
#include <iostream>
using namespace ns3;
NS_LOG_COMPONENT_DEFINE("SimpleTestApp");
void myprinter(NodeContainer allNodes)
{
	//Ptr<Node> node = Names::Find<Node>("Node12");
	//NS_LOG_DEBUG(*node->GetObject<ndn::Fib>());
	NS_LOG_DEBUG(allNodes.GetN());
	for (int i=0; i<allNodes.GetN(); i++) {
		NS_LOG_DEBUG(Names::FindName(allNodes.Get(i)));
	}
	Simulator::Schedule(Seconds(1), myprinter, allNodes);
}
int main(int argc, char** argv)
{
	Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
	Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
	Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));
	CommandLine cmd;
	cmd.Parse(argc, argv);
	AnnotatedTopologyReader topoReader("", 25);
	topoReader.SetFileName("topo/line3.txt");
	/*
    topoReader.Read();
	UserReader userReader;
	userReader.SetFilename("topo/user.txt");
	userReader.Read();
    */

    NS_LOG_DEBUG("00000");
	NodeContainer allNodes = topoReader.GetNodes();

	ndn::AppHelper server("ServerApp");
	ndn::AppHelper client("ClientApp");
	NS_LOG_DEBUG("test--1");
	client.Install(Names::Find<Node>("Node1"));
	NS_LOG_DEBUG("test--2");
	server.Install(Names::Find<Node>("Node2"));
	server.Install(Names::Find<Node>("Node3"));
	NS_LOG_DEBUG("JGLDSJL");

	ndn::GlobalRoutingHelper routingHelper;
	routingHelper.InstallAll();
	routingHelper.AddOrigins("/prefix", Names::Find<Node>("Node2"));
	routingHelper.AddOrigins("/prefix", Names::Find<Node>("Node3"));
	ndn::GlobalRoutingHelper::CalculateRoutes();

	NodeContainer container;
	container.Add(Names::Find<Node>("Node11"));
	container.Add(Names::Find<Node>("Node13"));
	container.Add(Names::Find<Node>("Node23"));

	Simulator::Schedule(Seconds(1), myprinter, container);


	Simulator::Stop(Seconds(20.0));
	ndn::L3AggregateTracer::InstallAll("aggregate-trace.txt", Seconds(1.0));
	ndn::L3RateTracer::InstallAll("rate-trace.txt", Seconds(1.0));
	L2RateTracer::InstallAll("drop-trace.txt", Seconds(1.0));
	Simulator::Run();
	Simulator::Destroy();
}
