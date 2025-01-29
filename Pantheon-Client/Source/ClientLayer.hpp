#pragma once

#include "Utopia/Application.hpp"
#include "Utopia/Layer.hpp"

#include "Utopia/Networking/Client.hpp"

#include <glm/glm.hpp>

#include "Renderer/Renderer.hpp"

namespace Pantheon {

	class ClientLayer : public Utopia::Layer
	{
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(float ts) override;
		virtual void OnRender() override;
		virtual void OnUIRender() override;
	private:
		void OnDataReceived(const Utopia::Buffer buffer);
	private:
		Renderer m_Renderer;

		glm::vec2 m_PlayerPosition{ 50, 50 };
		glm::vec2 m_PlayerVelocity{ 0, 0 };

		std::string m_ServerAddress;

		Utopia::Client m_Client;
		uint32_t m_PlayerID = 0;

		struct PlayerData
		{
			glm::vec2 Position;
			glm::vec2 Velocity;
		};

		std::mutex m_PlayerDataMutex;
		std::map<uint32_t, PlayerData> m_PlayerData;
	};

}