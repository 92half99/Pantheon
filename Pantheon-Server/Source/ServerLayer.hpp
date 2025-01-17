#pragma once

#include "Utopia/Layer.hpp"

#include "HeadlessConsole.hpp"

#include "Utopia/Networking/Server.hpp"

#include "glm/glm.hpp"

#include <map>

namespace Pantheon {

	class ServerLayer : public Utopia::Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float ts) override;
		virtual void OnUIRender() override;
	private:
		void OnConsoleMessage(std::string_view message);

		void OnClientConnected(const Utopia::ClientInfo& clientInfo);
		void OnClientDisconnected(const Utopia::ClientInfo& clientInfo);
		void OnDataReceived(const Utopia::ClientInfo& clientInfo, const Utopia::Buffer buffer);
	private:
		HeadlessConsole m_Console;
		Utopia::Server m_Server{ 8192 };

		struct PlayerData
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};


}