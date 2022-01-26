#include "lightSystem.h"
#include <Gengine/errors.h>
#include "world.h"

lightSystem::lightSystem()
{
}


lightSystem::~lightSystem()
{
}

void luaHandler::_registerLightFunction() {
	luaL_Reg lightlib_m[] = {
		{ "setColor", LUA_LIGHT_setColor },
		{ "setIntensity", LUA_LIGHT_setIntensity },
		{ "setRadius", LUA_LIGHT_setRadius },
		{ "setDirection", LUA_LIGHT_setDirection },
		{ "setOffset", LUA_LIGHT_setOffset },
		{ "setConeAngle", LUA_LIGHT_setConeAngle },
		{ "getColor", LUA_LIGHT_getColor },
		{ "getIntensity", LUA_LIGHT_getIntensity },
		{ "getRadius", LUA_LIGHT_getRadius },
		{ "getDirection", LUA_LIGHT_getDirection },
		{ "getConeAngle", LUA_LIGHT_getConeAngle },
		{ NULL, NULL }
	};

	luaL_newmetatable(_S, "light_m");

	lua_pushstring(_S, "__index");
	lua_pushvalue(_S, -2);  /* pushes the metatable*/
	lua_settable(_S, -3);  /* metatable.__index = metatable */

	luaL_setfuncs(_S, lightlib_m, 0);
}

int luaHandler::LUA_LIGHT_setColor(lua_State *S){
	light* p = *((light**)lua_touserdata(S, 1));
	p->color = gengine::color(lua_tonumber(S, 2), lua_tonumber(S, 3), lua_tonumber(S, 4), lua_tonumber(S, 5));
	p->calculateTypeID();
	return 0;
}
int luaHandler::LUA_LIGHT_setIntensity(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	p->intensity = lua_tonumber(S, 2);
	p->calculateTypeID();
	return 0;
}
int luaHandler::LUA_LIGHT_setRadius(lua_State *S){
	light* p = *((light**)lua_touserdata(S, 1));
	p->setRadius(lua_tonumber(S, 2), _world->getB2World());
	p->calculatePoints(_world->getB2World());
	return 0;
}
int luaHandler::LUA_LIGHT_setDirection(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	p->lightDirection = glm::vec2(lua_tonumber(S,2),lua_tonumber(S,3));
	return 0;
}
int luaHandler::LUA_LIGHT_setOffset(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	p->setOffset(glm::vec2(globals::pixToMet(lua_tonumber(S, 2)), globals::pixToMet(lua_tonumber(S, 3))));
	return 0;
}
int luaHandler::LUA_LIGHT_setConeAngle(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	p->coneAngle = lua_tonumber(S, 2);
	return 0;
}

int luaHandler::LUA_LIGHT_getColor(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	lua_pushnumber(S, p->color.r);
	lua_pushnumber(S, p->color.g);
	lua_pushnumber(S, p->color.b);
	lua_pushnumber(S, p->color.a);
	return 4;
}
int luaHandler::LUA_LIGHT_getIntensity(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	lua_pushnumber(S, p->intensity);
	return 1;
}
int luaHandler::LUA_LIGHT_getRadius(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	lua_pushnumber(S, p->getRadius());
	return 1;
}
int luaHandler::LUA_LIGHT_getDirection(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	lua_pushnumber(S, p->lightDirection.x);
	lua_pushnumber(S, p->lightDirection.y);
	return 2;
}

int luaHandler::LUA_LIGHT_getConeAngle(lua_State *S) {
	light* p = *((light**)lua_touserdata(S, 1));
	lua_pushnumber(S, p->coneAngle);
	return 1;
}

void lightSystem::initGLSLPrograms() {
	_copyShader.compileShaders("shaders/copyShader.vert", "shaders/copyShader.frag");
	_copyShader.addAttribute("vertexPosition");
	_copyShader.addAttribute("vertexColor");
	_copyShader.addAttribute("vertexUV");
	_copyShader.linkShaders();

	_lightProgram.compileShaders("shaders/lightShading.vert", "shaders/lightShading.frag");
	_lightProgram.addAttribute("vertexPosition");
	_lightProgram.addAttribute("vertexColor");
	_lightProgram.addAttribute("vertexUV");
	_lightProgram.linkShaders();

	_lightApplyProgram.compileShaders("shaders/lightApply.vert", "shaders/lightApply.frag");
	_lightApplyProgram.addAttribute("vertexPosition");
	_lightApplyProgram.addAttribute("vertexColor");
	_lightApplyProgram.addAttribute("vertexUV");
	_lightApplyProgram.linkShaders();

	_horizontalBlurProgram.compileShaders("shaders/horizontalBlur.vert", "shaders/horizontalBlur.frag");
	_horizontalBlurProgram.addAttribute("vertexPosition");
	_horizontalBlurProgram.addAttribute("vertexColor");
	_horizontalBlurProgram.addAttribute("vertexUV");
	_horizontalBlurProgram.linkShaders();

	_verticalBlurProgram.compileShaders("shaders/verticalBlur.vert", "shaders/verticalBlur.frag");
	_verticalBlurProgram.addAttribute("vertexPosition");
	_verticalBlurProgram.addAttribute("vertexColor");
	_verticalBlurProgram.addAttribute("vertexUV");
	_verticalBlurProgram.linkShaders();

	_largeVerticalBlurProgram.compileShaders("shaders/largeVerticalBlur.vert", "shaders/largeVerticalBlur.frag");
	_largeVerticalBlurProgram.addAttribute("vertexPosition");
	_largeVerticalBlurProgram.addAttribute("vertexColor");
	_largeVerticalBlurProgram.addAttribute("vertexUV");
	_largeVerticalBlurProgram.linkShaders();

	_largeHorizontalBlurProgram.compileShaders("shaders/largeHorizontalBlur.vert", "shaders/largeHorizontalBlur.frag");
	_largeHorizontalBlurProgram.addAttribute("vertexPosition");
	_largeHorizontalBlurProgram.addAttribute("vertexColor");
	_largeHorizontalBlurProgram.addAttribute("vertexUV");
	_largeHorizontalBlurProgram.linkShaders();

	_ambiantLightProgram.compileShaders("shaders/ambiantLight.vert", "shaders/ambiantLight.frag");
	_ambiantLightProgram.addAttribute("vertexPosition");
	_ambiantLightProgram.addAttribute("vertexColor");
	_ambiantLightProgram.addAttribute("vertexUV");
	_ambiantLightProgram.linkShaders();
}

void lightSystem::useProgram() {
	_lightProgram.use();
}

void lightSystem::unuseProgram() {
	_lightProgram.unuse();
}

void lightSystem::createBuffers() {
	_createFrameBuffer();
	_createShadowMap();
	_createBlurBuffer();
	_createTileBuffer();
	_createDownsamplingBuffer();
}

void lightSystem::_createFrameBuffer() {
	glGenFramebuffers(1, &colorFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFrameBuffer);

	glDisable(GL_DEPTH_TEST);

	// The texture we're going to render to
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, FRAME_BUFFER_RESOLUTION.x, FRAME_BUFFER_RESOLUTION.y, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	_lightApplyProgram.use();

	colorSamplerLoc = _lightApplyProgram.getUniformLocation("colorSampler");

	_lightApplyProgram.unuse();

	// Poor filtering. Needed !
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, frameBufferTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		gengine::fatalError("Error creating render frame buffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void lightSystem::_createShadowMap() {
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	// The texture we're going to render to
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SHADOW_MAP_RESOLUTION.x, SHADOW_MAP_RESOLUTION.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_INTERPOLATE);
	_lightApplyProgram.use();

	shadowSamplerLoc = _lightApplyProgram.getUniformLocation("lightSampler");
	_lightApplyProgram.unuse();

	// Poor filtering. Needed !
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowMap, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		gengine::fatalError("Error creating shadow frame buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void lightSystem::_createBlurBuffer() {
	glGenFramebuffers(1, &horizontalFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, horizontalFrameBuffer);

	// The texture we're going to render to
	glGenTextures(1, &horizontalBlurTexture);
	glBindTexture(GL_TEXTURE_2D, horizontalBlurTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BLUR_BUFFER_RESOLUTION.x, BLUR_BUFFER_RESOLUTION.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_INTERPOLATE);

	_horizontalBlurProgram.use();

	hBlurSamplerLoc = _horizontalBlurProgram.getUniformLocation("lightSampler");
	_horizontalBlurProgram.unuse();

	_verticalBlurProgram.use();

	vBlurSamplerLoc = _verticalBlurProgram.getUniformLocation("lightSampler");
	_verticalBlurProgram.unuse();

	// Poor filtering. Needed !
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, horizontalBlurTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		gengine::fatalError("Error creating blur frame buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void lightSystem::_createTileBuffer() {
	glGenFramebuffers(1, &tileFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, tileFrameBuffer);

	// The texture we're going to render to
	glGenTextures(1, &tileTexture);
	glBindTexture(GL_TEXTURE_2D, tileTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TILE_BUFFER_RESOLUTION.x, TILE_BUFFER_RESOLUTION.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_INTERPOLATE);
	_ambiantLightProgram.use();

	tileSamplerLoc = _ambiantLightProgram.getUniformLocation("tileSampler");
	shadowSampler2Loc = _ambiantLightProgram.getUniformLocation("lightSampler");

	_ambiantLightProgram.unuse();

	// Poor filtering. Needed !
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tileTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		gengine::fatalError("Error creating tile frame buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void lightSystem::_createDownsamplingBuffer() {
	glGenFramebuffers(1, &downsamplingFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, downsamplingFrameBuffer);

	// The texture we're going to render to
	glGenTextures(1, &downsamplingTexture);
	glBindTexture(GL_TEXTURE_2D, downsamplingTexture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DOWNSAMPLING_BUFFER_RESOLUTION.x, DOWNSAMPLING_BUFFER_RESOLUTION.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_INTERPOLATE);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, downsamplingTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		gengine::fatalError("Error creating tile frame buffer");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLint lightSystem::getUniformLocation(std::string location) {
	return _lightProgram.getUniformLocation(location);
}
GLint lightSystem::lightApplyGetUniformLocation(std::string location) {
	return _lightApplyProgram.getUniformLocation(location);
}
GLint lightSystem::ambiantLightGetUniformLocation(std::string location) {
	return _ambiantLightProgram.getUniformLocation(location);
}


void lightSystem::renderToRenderBuffer() {
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, colorFrameBuffer);

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, FRAME_BUFFER_RESOLUTION.x, FRAME_BUFFER_RESOLUTION.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void lightSystem::renderToShadowBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, SHADOW_MAP_RESOLUTION.x, SHADOW_MAP_RESOLUTION.y);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
}

void lightSystem::copyToShadowBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	glViewport(0, 0, SHADOW_MAP_RESOLUTION.x, SHADOW_MAP_RESOLUTION.y);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void lightSystem::copyToShadowBufferAndClear() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, SHADOW_MAP_RESOLUTION.x, SHADOW_MAP_RESOLUTION.y);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void lightSystem::copyToDownsamplingBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, downsamplingFrameBuffer);

	glViewport(0, 0, DOWNSAMPLING_BUFFER_RESOLUTION.x, DOWNSAMPLING_BUFFER_RESOLUTION.y);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void lightSystem::renderToBlurBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, horizontalFrameBuffer);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, BLUR_BUFFER_RESOLUTION.x, BLUR_BUFFER_RESOLUTION.y);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);
}

void lightSystem::renderToTileBuffer() {
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, tileFrameBuffer);

	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, TILE_BUFFER_RESOLUTION.x, TILE_BUFFER_RESOLUTION.y);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void lightSystem::unbindBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, FRAME_BUFFER_RESOLUTION.x, FRAME_BUFFER_RESOLUTION.y);
	glClear(GL_COLOR_BUFFER_BIT);
}

void lightSystem::downSampleShadowBuffer(gengine::spriteBatch* spriteBatch) {
	{
		_copyShader.use();

		copyToDownsamplingBuffer();

		spriteBatch->begin();

		spriteBatch->draw(glm::vec4(0,0,1,1), glm::vec4(0, 0, 1.0f, 1.0f), shadowMap, 1, gengine::color(255, 255, 255, 255));

		spriteBatch->end();
		spriteBatch->renderBatches();

		_copyShader.unuse();

		blurDownsampledTexture(spriteBatch);

		_copyShader.use();

		copyToShadowBufferAndClear();

		spriteBatch->begin();

		spriteBatch->draw(glm::vec4(0,0,1,1), glm::vec4(0, 0, 1.0f, 1.0f), downsamplingFrameBuffer, 1, gengine::color(255, 255, 255, 255));

		spriteBatch->end();
		spriteBatch->renderBatches();

		_copyShader.unuse();

	}
}

void lightSystem::addAmbiantLightToShadowMap(gengine::spriteBatch* spriteBatch, gengine::color ambiantLightColor, gengine::GLSLProgram* textureProgram, gengine::camera2D* camera) {
	_ambiantLightProgram.use();

	GLint ambiantLightColorLoc = _ambiantLightProgram.getUniformLocation("ambientLightColor");
	glUniform4f(ambiantLightColorLoc, (float)ambiantLightColor.r / 255.0f, (float)ambiantLightColor.g / 255.0f, (float)ambiantLightColor.b / 255.0f, (float)ambiantLightColor.a / 255.0f);

	copyToShadowBuffer();

	glUniform1i(shadowSampler2Loc, 1);

	glUniform1i(tileSamplerLoc, 2);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, tileTexture);

	glActiveTexture(GL_TEXTURE0);

	glm::vec4 destRect;
	destRect.x = 0.0f;
	destRect.y = 0.0f;
	destRect.z = 1.0f;
	destRect.w = 1.0f;

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0, 0, 1, 1), 0, 1, gengine::color(255, 255, 255, 255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_ambiantLightProgram.unuse();
}

void lightSystem::blurShadows(gengine::spriteBatch* spriteBatch) {
	//Reduce the quality of the shadow buffer to create more blur
	downSampleShadowBuffer(spriteBatch);
	downSampleShadowBuffer(spriteBatch);
	downSampleShadowBuffer(spriteBatch);

	//********************************************Horizontal blur******************************************//
	_horizontalBlurProgram.use();

	glm::vec4 destRect;
	destRect.x = 0.0f;
	destRect.y = 0.0f;
	destRect.z = 1.0f;
	destRect.w = 1.0f;

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(hBlurSamplerLoc, 0);

	renderToBlurBuffer();

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0, 0, 1.0f, 1.0f), shadowMap, 1, gengine::color(255, 255, 255, 255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_horizontalBlurProgram.unuse();

	//********************************************Vertical blur******************************************//

	_verticalBlurProgram.use();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(vBlurSamplerLoc, 0);

	copyToShadowBufferAndClear();

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0, 0, 1.0f, 1.0f), horizontalFrameBuffer, 1, gengine::color(255, 255, 255, 255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_verticalBlurProgram.unuse();

	unbindBuffer();
}

void lightSystem::blurDownsampledTexture(gengine::spriteBatch* spriteBatch) {
	//********************************************Horizontal blur******************************************//
	_largeHorizontalBlurProgram.use();

	glm::vec4 destRect;
	destRect.x = 0.0f;
	destRect.y = 0.0f;
	destRect.z = 1.0f;
	destRect.w = 1.0f;

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(hBlurSamplerLoc, 0);

	renderToBlurBuffer();

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), downsamplingTexture, 1, gengine::color(255, 255, 255, 255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_largeHorizontalBlurProgram.unuse();

	//********************************************Vertical blur******************************************//

	_largeVerticalBlurProgram.use();

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(vBlurSamplerLoc, 0);

	copyToDownsamplingBuffer();

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0, 0, 1.0f, 1.0f), horizontalFrameBuffer, 1, gengine::color(255, 255, 255, 255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_largeVerticalBlurProgram.unuse();
}

void lightSystem::renderProduct(gengine::spriteBatch* spriteBatch, gengine::GLSLProgram* textureProgram) {
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ZERO);

	unbindBuffer();

	_lightApplyProgram.use();

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);

	glUniform1i(colorSamplerLoc, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glUniform1i(shadowSamplerLoc, 4);

	glActiveTexture(GL_TEXTURE0);

	glm::vec4 destRect;
	destRect.x = 0.0f;
	destRect.y = 0.0f;
	destRect.z = 1.0f;
	destRect.w = 1.0f;

	spriteBatch->begin();

	spriteBatch->draw(destRect, glm::vec4(0,0,1.0f,1.0f) , 0, 1, gengine::color(255,255,255,255), 0);

	spriteBatch->end();
	spriteBatch->renderBatches();

	_lightApplyProgram.unuse();
}