#include "test-app.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"

#include "ns3/ndn-app-face.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"

#include "ns3/ndn-fib.h"
#include "ns3/random-variable.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/core-module.h"

NS_LOG_COMPONENT_DEFINE("TestApp");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(ServerApp);
NS_OBJECT_ENSURE_REGISTERED(ClientApp);

TypeId ServerApp::GetTypeId()
{
	static TypeId tid = TypeId("ServerApp")
		.SetParent<ndn::App>()
		.AddConstructor<ServerApp>()
		;
	return tid;
}
void ServerApp::StartApplication()
{
	ndn::App::StartApplication();
	NS_LOG_DEBUG("start server");
	Ptr<ndn::Name> prefix = Create<ndn::Name>("/prefix");
	Ptr<ndn::Fib> fib = GetNode()->GetObject<ndn::Fib>();
	fib->Add(*prefix, m_face, 0);
}
void ServerApp::StopApplication()
{
	ndn::App::StopApplication();
}
void ServerApp::OnInterest(Ptr<const ndn::Interest> interest)
{
	ndn::App::OnInterest(interest);
	NS_LOG_DEBUG("Received Interest packet for " << interest->GetName());
}
void ServerApp::OnData(Ptr<const ndn::Data> contentObject)
{
	NS_LOG_DEBUG("Receiving Data packet for " << contentObject->GetName());
	std::cout << "DATA received for name " << contentObject->GetName() << std::endl;
}

TypeId ClientApp::GetTypeId()
{
	static TypeId tid = TypeId("ClientApp")
		.SetParent<ndn::App>()
		.AddConstructor<ClientApp>()
		;
	return tid;
}
void ClientApp::StartApplication()
{
	ndn::App::StartApplication();
	NS_LOG_DEBUG("start client");
	Simulator::Schedule(Seconds(1), &ClientApp::Period, this);
}
void ClientApp::StopApplication()
{
	ndn::App::StopApplication();
}
void ClientApp::OnInterest(Ptr<const ndn::Interest> interest)
{
	ndn::App::OnInterest(interest);
	NS_LOG_DEBUG("Received Interest packet for " << interest->GetName());
}
void ClientApp::OnData(Ptr<const ndn::Data> contentObject)
{
	NS_LOG_DEBUG("Receiving Data packet for " << contentObject->GetName());
	std::cout << "DATA received for name " << contentObject->GetName() << std::endl;
}
void ClientApp::SendInterest(const std::string &name)
{
	Ptr<ndn::Interest> interest = Create<ndn::Interest>();
	Ptr<ndn::Name> prefix = Create<ndn::Name>(name);
	UniformVariable rand(0, std::numeric_limits<uint32_t>::max());
	interest->SetNonce(rand.GetValue());
	interest->SetName(prefix);
	interest->SetInterestLifetime(Seconds(1));
	NS_LOG_DEBUG("Sending interest " << *prefix);
	m_transmittedInterests(interest, this, m_face);
	m_face->ReceiveInterest(interest);
}
void ClientApp::Period()
{
	SendInterest("/prefix");
	Simulator::Schedule(Seconds(1), &ClientApp::Period, this);
}

} //ns3
