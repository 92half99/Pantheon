#include "Utopia/Application.hpp"
#include "Utopia/EntryPoint.hpp"

#include "ClientLayer.hpp"

Utopia::Application* Utopia::CreateApplication(int argc, char** argv)
{
	Utopia::ApplicationSpecification spec;
	spec.Name = "Pantheon Example";
	spec.CustomTitlebar = false;
	spec.UseDockspace = false;

	Utopia::Application* app = new Utopia::Application(spec);
	app->PushLayer<Pantheon::ClientLayer>();
	return app;
}