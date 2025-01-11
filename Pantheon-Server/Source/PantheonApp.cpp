#include "Utopia/Application.hpp"
#include "Utopia/EntryPoint.hpp"

#include "Utopia/Core/Log.hpp"

#include "ServerLayer.hpp"

Utopia::Application* Utopia::CreateApplication(int argc, char** argv)
{
	Utopia::ApplicationSpecification spec;
	spec.Name = "Pantheon Server";

	Utopia::Application* app = new Utopia::Application(spec);
	app->PushLayer<Pantheon::ServerLayer>();
	return app;
}