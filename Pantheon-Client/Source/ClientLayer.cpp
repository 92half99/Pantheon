#include "ClientLayer.hpp"

#include "Utopia/Input/Input.hpp"
#include "Utopia/ImGui/ImGuiTheme.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Utopia/Serialization/BufferStream.hpp"

#include "ServerPacket.hpp"

namespace Pantheon {

	static Utopia::Buffer s_ScratchBuffer;

	static void DrawRect(glm::vec2 position, glm::vec2 size, uint32_t color)
	{
		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImVec2 min = ImGui::GetWindowPos() + ImVec2(position.x, position.y);
		ImVec2 max = min + ImVec2(size.x, size.y);

		drawList->AddRectFilled(min, max, color);
	}

	void ClientLayer::OnAttach()
	{
		s_ScratchBuffer.Allocate(10 * 1024 * 1024); // 10 MB

		m_Client.SetDataReceivedCallback([this](const Utopia::Buffer buffer) { OnDataReceived(buffer); });
	}

	void ClientLayer::OnDetach()
	{

	}

	void ClientLayer::OnUpdate(float ts)
	{
		if (m_Client.GetConnectionStatus() != Utopia::Client::ConnectionStatus::Connected)
			return;

		glm::vec2 dir{ 0.0f, 0.0f };

		if (Utopia::Input::IsKeyDown(Utopia::KeyCode::W))
			dir.y = -1;
		else if (Utopia::Input::IsKeyDown(Utopia::KeyCode::S))
			dir.y = 1;

		if (Utopia::Input::IsKeyDown(Utopia::KeyCode::A))
			dir.x = -1;
		else if (Utopia::Input::IsKeyDown(Utopia::KeyCode::D))
			dir.x = 1;

		// Optional (for gameplay reasons?)
		if (glm::length(dir) > 0.0f)
		{
			const float speed = 150.0f;
			dir = glm::normalize(dir);
			m_PlayerVelocity = dir * speed;
		}

		m_PlayerPosition += m_PlayerVelocity * ts;

		m_PlayerVelocity = glm::mix(m_PlayerVelocity, glm::vec2(0.0f), 10.0f * ts);


		Utopia::BufferStreamWriter stream(s_ScratchBuffer);
		stream.WriteRaw(PacketType::ClientUpdate);
		stream.WriteRaw<glm::vec2>(m_PlayerPosition);
		stream.WriteRaw<glm::vec2>(m_PlayerVelocity);
		m_Client.SendBuffer(stream.GetBuffer());

	}

	void ClientLayer::OnUIRender()
	{
		Utopia::Client::ConnectionStatus connectionStatus = m_Client.GetConnectionStatus();
		if (connectionStatus == Utopia::Client::ConnectionStatus::Connected)
		{
			// play game
			DrawRect(m_PlayerPosition, { 50.0f, 50.0f }, 0xffff00ff);

			m_PlayerDataMutex.lock();
			std::map<uint32_t, PlayerData> playerData = m_PlayerData;
			m_PlayerDataMutex.unlock();

			for (const auto& [id, data] : playerData)
			{
				if (id == m_PlayerID)
					continue;

				DrawRect(data.Position, { 50.0f, 50.0f }, 0xff00ff00);
			}
		}
		else
		{
			ImGui::Begin("Connect to Server");

			ImGui::InputText("Server address", &m_ServerAddress);
			if (connectionStatus == Utopia::Client::ConnectionStatus::FailedToConnect)
				ImGui::TextColored(ImColor(Utopia::UI::Colors::Theme::error), "Failed to connect.");
			else if (connectionStatus == Utopia::Client::ConnectionStatus::Connecting)
				ImGui::TextColored(ImColor(Utopia::UI::Colors::Theme::textDarker), "Connecting...");
			if (ImGui::Button("Connect"))
			{
				m_Client.ConnectToServer(m_ServerAddress);
			}

			ImGui::End();
		}

		ImGui::ShowDemoWindow();

	}

	void ClientLayer::OnDataReceived(const Utopia::Buffer buffer)
	{
		Utopia::BufferStreamReader stream(buffer);

		PacketType type;
		stream.ReadRaw(type);
		switch (type)
		{
		case PacketType::ClientConnect:

			uint32_t idFromServer;
			stream.ReadRaw<uint32_t>(idFromServer);
			UT_INFO("We have connected! Server says our ID is {}", idFromServer);
			// UT_INFO("We say our ID is {}", m_Client.GetID());

			m_PlayerID = idFromServer;
			break;

		case PacketType::ClientUpdate:
			
			m_PlayerDataMutex.lock();
			stream.ReadMap(m_PlayerData);
			m_PlayerDataMutex.unlock();

			break;
		}

	}

}