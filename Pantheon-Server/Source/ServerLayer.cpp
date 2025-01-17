#include "ServerLayer.hpp"

#include <chrono>

#include "Utopia/Core/Log.hpp"
#include "Utopia/Serialization/BufferStream.hpp"

#include "ServerPacket.hpp"

namespace Pantheon {

	static Utopia::Buffer s_ScratchBuffer;

	void ServerLayer::OnAttach()
	{
		s_ScratchBuffer.Allocate(10 * 1024 * 1024); // 10 MB

		m_Console.SetMessageSendCallback([this](std::string_view message) { OnConsoleMessage(message); });

		m_Server.SetClientConnectedCallback([this](const Utopia::ClientInfo& clientInfo) { OnClientConnected(clientInfo); });
		m_Server.SetClientDisconnectedCallback([this](const Utopia::ClientInfo& clientInfo) { OnClientDisconnected(clientInfo); });
		m_Server.SetDataReceivedCallback([this](const Utopia::ClientInfo& clientInfo, const Utopia::Buffer buffer) { OnDataReceived(clientInfo, buffer); });

		m_Server.Start();
	}

	void ServerLayer::OnDetach()
	{
		m_Server.Stop();
	}

	void ServerLayer::OnUpdate(float ts)
	{
		Utopia::BufferStreamWriter stream(s_ScratchBuffer);
		stream.WriteRaw(PacketType::ClientUpdate);
		m_PlayerDataMutex.lock();
		{
			stream.WriteMap(m_PlayerData);
		}
		m_PlayerDataMutex.unlock();

		m_Server.SendBufferToAllClients(stream.GetBuffer());

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5ms);

	}

	void ServerLayer::OnUIRender()
	{
	}

	void ServerLayer::OnConsoleMessage(std::string_view message)
	{
		if (message.starts_with('/'))
		{
			// command
			std::cout << "You called the " << message << " command!\n";
		}
	}

	void ServerLayer::OnClientConnected(const Utopia::ClientInfo& clientInfo)
	{
		UT_INFO_TAG("Server", "Client connected! ID={}", clientInfo.ID);
		std::cout << "[Server] Client connected! ID=" << clientInfo.ID << '\n';

		Utopia::BufferStreamWriter stream(s_ScratchBuffer);

		stream.WriteRaw(PacketType::ClientConnect);
		stream.WriteRaw(clientInfo.ID);
		m_Server.SendBufferToClient(clientInfo.ID, stream.GetBuffer());
	}

	void ServerLayer::OnClientDisconnected(const Utopia::ClientInfo& clientInfo)
	{
		UT_INFO_TAG("Server", "Client disconnected! ID={}", clientInfo.ID);
		std::cout << "[Server] Client disconnected! ID=" << clientInfo.ID << '\n';
	}

	void ServerLayer::OnDataReceived(const Utopia::ClientInfo& clientInfo, const Utopia::Buffer buffer)
	{
		Utopia::BufferStreamReader stream(buffer);
		
		PacketType type;
		stream.ReadRaw(type);
		switch (type)
		{
		case PacketType::ClientUpdate:
			
			m_PlayerDataMutex.lock();
			{
				PlayerData& playerData = m_PlayerData[clientInfo.ID];
				stream.ReadRaw<glm::vec2>(playerData.Position);
				stream.ReadRaw<glm::vec2>(playerData.Velocity);
			}
			m_PlayerDataMutex.unlock();

			break;
		}

	}

}