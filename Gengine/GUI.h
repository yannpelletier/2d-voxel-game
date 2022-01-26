#pragma once
#include <glm\glm.hpp>
#include <SDL/SDL_events.h>
#include <SDL/SDL_timer.h>
#include <CEGUI\CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GL3Renderer.h>

namespace gengine {
	class GUI
	{
	public:
		void init(const std::string& ressourceDirectory);
		void destroy();

		void draw();
		void update();

		void setMouseCursor(const std::string& imageFile);
		void showMouseCursor();
		void hideMouseCursor();

		void onSDLEvent(SDL_Event& evnt);

		void loadScheme(const std::string& schemeFile);
		void setFont(const std::string& fontFile);
		CEGUI::Window* createWidget(const std::string& type, const glm::vec4& destRectPercentage, const glm::vec4& destRectPixels, const std::string& name = "");
		CEGUI::Window* createWidget(const std::string& type,const std::string& name = "");
		static void setWidgetDestRect(CEGUI::Window* window, const glm::vec4& destRectPercentage, const glm::vec4& destRectPixels);

		static CEGUI::OpenGL3Renderer* getRenderer() { return _renderer; }
		const CEGUI::GUIContext* getContext() { return _context; }
		CEGUI::Window* getRoot() { return _rootWindow; }

		bool hasClickPriority();
	private:
		static CEGUI::OpenGL3Renderer* _renderer;
		CEGUI::GUIContext* _context;
		CEGUI::Window* _rootWindow = nullptr;

		unsigned int _lastTime = 0;
		bool _hasClickPriority = false;
	};

}
