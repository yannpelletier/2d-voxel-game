#include "GUI.h"
#include <iostream>

namespace gengine {

	CEGUI::OpenGL3Renderer* GUI::_renderer = nullptr;

	void GUI::init(const std::string& ressourceDirectory)
	{
		if (_renderer == nullptr) {
			_renderer = &CEGUI::OpenGL3Renderer::bootstrapSystem();

			CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());
			rp->setResourceGroupDirectory("imageSets", ressourceDirectory + "/imagesets/");
			rp->setResourceGroupDirectory("schemes", ressourceDirectory + "/schemes/");
			rp->setResourceGroupDirectory("fonts", ressourceDirectory + "/fonts/");
			rp->setResourceGroupDirectory("layouts", ressourceDirectory + "/layouts/");
			rp->setResourceGroupDirectory("looknfeels", ressourceDirectory + "/looknfeel/");
			rp->setResourceGroupDirectory("lua_scripts", ressourceDirectory + "/lua_scripts/");

			CEGUI::ImageManager::setImagesetDefaultResourceGroup("imageSets");
			CEGUI::Scheme::setDefaultResourceGroup("schemes");
			CEGUI::Font::setDefaultResourceGroup("fonts");
			CEGUI::WindowManager::setDefaultResourceGroup("layouts");
			CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
			CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");


		}

		_context = &CEGUI::System::getSingleton().createGUIContext(_renderer->getDefaultRenderTarget());
		_rootWindow = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "root");
		_context->setRootWindow(_rootWindow);
	}


	void GUI::destroy()
	{
		CEGUI::System::getSingleton().destroyGUIContext(*_context);
	}

	void GUI::draw() {
		_renderer->beginRendering();
		_context->draw();
		_renderer->endRendering();

		glDisable(GL_SCISSOR_TEST);
	}

	void GUI::update() {
		unsigned int elapsed;
		if (_lastTime == 0) {
			elapsed = 0;
			_lastTime = SDL_GetTicks();
		} {
			unsigned int newTime = SDL_GetTicks();
			elapsed = newTime - _lastTime;
			_lastTime = newTime;
		}

		_context->injectTimePulse((float)(elapsed / 1000.0f));

		_hasClickPriority = false;

		CEGUI::Sizef cSize = _context->getSurfaceSize();

		for (int i = 0; i < _rootWindow->getChildCount(); i++) {
			CEGUI::Vector2f mousePos = _context->getMouseCursor().getPosition();

			if (_rootWindow->getChildAtIdx(i)->getType() != "DragContainer") {
				if (_rootWindow->getChildAtIdx(i)->isVisible()) {
					CEGUI::URect windowArea = _rootWindow->getChildAtIdx(i)->getArea();

					if (((windowArea.d_min.d_x.d_offset + (windowArea.d_min.d_x.d_scale * cSize.d_width)) >  mousePos.d_x) ||
						((windowArea.d_max.d_x.d_offset + (windowArea.d_max.d_x.d_scale * cSize.d_width)) < mousePos.d_x) ||
						((windowArea.d_min.d_y.d_offset + (windowArea.d_min.d_y.d_scale * cSize.d_height)) >  mousePos.d_y) ||
						((windowArea.d_max.d_y.d_offset + (windowArea.d_max.d_y.d_scale * cSize.d_height)) < mousePos.d_y))
					{

					}
					else {
						_hasClickPriority = true;
						break;
					}
				}

			}
		}
	}

	void GUI::setMouseCursor(const std::string& imageFile) {
		_context->getMouseCursor().setDefaultImage(imageFile);
	}
	void GUI::showMouseCursor() {
		_context->getMouseCursor().show();
	}
	void GUI::hideMouseCursor() {
		_context->getMouseCursor().hide();
	}

	CEGUI::Key::Scan SDLKeyToCEGUIKey(SDL_Keycode key)
	{
		using namespace CEGUI;
		switch (key)
		{
		case SDLK_BACKSPACE:    return Key::Backspace;
		case SDLK_TAB:          return Key::Tab;
		case SDLK_RETURN:       return Key::Return;
		case SDLK_PAUSE:        return Key::Pause;
		case SDLK_ESCAPE:       return Key::Escape;
		case SDLK_SPACE:        return Key::Space;
		case SDLK_COMMA:        return Key::Comma;
		case SDLK_MINUS:        return Key::Minus;
		case SDLK_PERIOD:       return Key::Period;
		case SDLK_SLASH:        return Key::Slash;
		case SDLK_0:            return Key::Zero;
		case SDLK_1:            return Key::One;
		case SDLK_2:            return Key::Two;
		case SDLK_3:            return Key::Three;
		case SDLK_4:            return Key::Four;
		case SDLK_5:            return Key::Five;
		case SDLK_6:            return Key::Six;
		case SDLK_7:            return Key::Seven;
		case SDLK_8:            return Key::Eight;
		case SDLK_9:            return Key::Nine;
		case SDLK_COLON:        return Key::Colon;
		case SDLK_SEMICOLON:    return Key::Semicolon;
		case SDLK_EQUALS:       return Key::Equals;
		case SDLK_LEFTBRACKET:  return Key::LeftBracket;
		case SDLK_BACKSLASH:    return Key::Backslash;
		case SDLK_RIGHTBRACKET: return Key::RightBracket;
		case SDLK_a:            return Key::A;
		case SDLK_b:            return Key::B;
		case SDLK_c:            return Key::C;
		case SDLK_d:            return Key::D;
		case SDLK_e:            return Key::E;
		case SDLK_f:            return Key::F;
		case SDLK_g:            return Key::G;
		case SDLK_h:            return Key::H;
		case SDLK_i:            return Key::I;
		case SDLK_j:            return Key::J;
		case SDLK_k:            return Key::K;
		case SDLK_l:            return Key::L;
		case SDLK_m:            return Key::M;
		case SDLK_n:            return Key::N;
		case SDLK_o:            return Key::O;
		case SDLK_p:            return Key::P;
		case SDLK_q:            return Key::Q;
		case SDLK_r:            return Key::R;
		case SDLK_s:            return Key::S;
		case SDLK_t:            return Key::T;
		case SDLK_u:            return Key::U;
		case SDLK_v:            return Key::V;
		case SDLK_w:            return Key::W;
		case SDLK_x:            return Key::X;
		case SDLK_y:            return Key::Y;
		case SDLK_z:            return Key::Z;
		case SDLK_DELETE:       return Key::Delete;
		/*case SDLK_KP0:          return Key::Numpad0;
		case SDLK_KP1:          return Key::Numpad1;
		case SDLK_KP2:          return Key::Numpad2;
		case SDLK_KP3:          return Key::Numpad3;
		case SDLK_KP4:          return Key::Numpad4;
		case SDLK_KP5:          return Key::Numpad5;
		case SDLK_KP6:          return Key::Numpad6;
		case SDLK_KP7:          return Key::Numpad7;
		case SDLK_KP8:          return Key::Numpad8;
		case SDLK_KP9:          return Key::Numpad9;*/
		case SDLK_KP_PERIOD:    return Key::Decimal;
		case SDLK_KP_DIVIDE:    return Key::Divide;
		case SDLK_KP_MULTIPLY:  return Key::Multiply;
		case SDLK_KP_MINUS:     return Key::Subtract;
		case SDLK_KP_PLUS:      return Key::Add;
		case SDLK_KP_ENTER:     return Key::NumpadEnter;
		case SDLK_KP_EQUALS:    return Key::NumpadEquals;
		case SDLK_UP:           return Key::ArrowUp;
		case SDLK_DOWN:         return Key::ArrowDown;
		case SDLK_RIGHT:        return Key::ArrowRight;
		case SDLK_LEFT:         return Key::ArrowLeft;
		case SDLK_INSERT:       return Key::Insert;
		case SDLK_HOME:         return Key::Home;
		case SDLK_END:          return Key::End;
		case SDLK_PAGEUP:       return Key::PageUp;
		case SDLK_PAGEDOWN:     return Key::PageDown;
		case SDLK_F1:           return Key::F1;
		case SDLK_F2:           return Key::F2;
		case SDLK_F3:           return Key::F3;
		case SDLK_F4:           return Key::F4;
		case SDLK_F5:           return Key::F5;
		case SDLK_F6:           return Key::F6;
		case SDLK_F7:           return Key::F7;
		case SDLK_F8:           return Key::F8;
		case SDLK_F9:           return Key::F9;
		case SDLK_F10:          return Key::F10;
		case SDLK_F11:          return Key::F11;
		case SDLK_F12:          return Key::F12;
		case SDLK_F13:          return Key::F13;
		case SDLK_F14:          return Key::F14;
		case SDLK_F15:          return Key::F15;
		case SDLK_RSHIFT:       return Key::RightShift;
		case SDLK_LSHIFT:       return Key::LeftShift;
		case SDLK_RCTRL:        return Key::RightControl;
		case SDLK_LCTRL:        return Key::LeftControl;
		case SDLK_RALT:         return Key::RightAlt;
		case SDLK_LALT:         return Key::LeftAlt;
		case SDLK_SYSREQ:       return Key::SysRq;
		case SDLK_MENU:         return Key::AppMenu;
		case SDLK_POWER:        return Key::Power;
		default:                return Key::Unknown;
		}
	}

	CEGUI::MouseButton SDLButtonToCEGUIButton(Uint8 sdlButton) {
		switch (sdlButton) {
		case SDL_BUTTON_LEFT: return CEGUI::MouseButton::LeftButton;
		case SDL_BUTTON_RIGHT: return CEGUI::MouseButton::RightButton;
		case SDL_BUTTON_MIDDLE: return CEGUI::MouseButton::MiddleButton;
		case SDL_BUTTON_X1: return CEGUI::MouseButton::X1Button;
		case SDL_BUTTON_X2: return CEGUI::MouseButton::X2Button;
		}
	}

	void GUI::onSDLEvent(SDL_Event& evnt) {
		CEGUI::utf32 codePoint;
		CEGUI::String cs = CEGUI::String((CEGUI::utf8*) evnt.text.text);
		switch (evnt.type) {
		case SDL_MOUSEMOTION:
			_context->injectMousePosition(evnt.motion.x, evnt.motion.y);
			break;
		case SDL_KEYDOWN:
			_context->injectKeyDown(SDLKeyToCEGUIKey(evnt.key.keysym.sym));
			break;
		case SDL_KEYUP:
			_context->injectKeyUp(SDLKeyToCEGUIKey(evnt.key.keysym.sym));
			break;
		case SDL_TEXTINPUT:
			codePoint = 0;
			for (int i = 0; i < cs.size(); ++i) {
				_context->injectChar(cs[i]);
			}

			break;
		case SDL_MOUSEBUTTONDOWN:
			_context->injectMouseButtonDown(SDLButtonToCEGUIButton(evnt.button.button));
			break;
		case SDL_MOUSEBUTTONUP:
			_context->injectMouseButtonUp(SDLButtonToCEGUIButton(evnt.button.button));
			break;
		}
	}


	void GUI::loadScheme(const std::string& schemeFile) {
		CEGUI::SchemeManager::getSingleton().createFromFile(schemeFile);
	}

	void GUI::setFont(const std::string& fontFile) {
		CEGUI::FontManager::getSingleton().createFromFile(fontFile + ".font");
		_context->setDefaultFont(fontFile);
	}

	CEGUI::Window* GUI::createWidget(const std::string& type, const glm::vec4& destRectPercentage, const glm::vec4& destRectPixels, const std::string& name) {
		CEGUI::Window* newWindow = CEGUI::WindowManager::getSingletonPtr()->createWindow(type, name);
		_rootWindow->addChild(newWindow);
		setWidgetDestRect(newWindow, destRectPercentage, destRectPixels);
		return newWindow;
	}

	CEGUI::Window* GUI::createWidget(const std::string& type, const std::string& name) {
		CEGUI::Window* newWindow = CEGUI::WindowManager::getSingletonPtr()->createWindow(type, name);
		_rootWindow->addChild(newWindow);
		return newWindow;
	}


	void GUI::setWidgetDestRect(CEGUI::Window* window, const glm::vec4& destRectPercentage, const glm::vec4& destRectPixels) {
		window->setPosition(CEGUI::UVector2(CEGUI::UDim(destRectPercentage.x, destRectPixels.x), CEGUI::UDim(destRectPercentage.y, destRectPixels.y)));
		window->setSize(CEGUI::USize(CEGUI::UDim(destRectPercentage.z, destRectPixels.z), CEGUI::UDim(destRectPercentage.w, destRectPixels.w)));
	}

	bool GUI::hasClickPriority() {
		return _hasClickPriority;
	}
}

