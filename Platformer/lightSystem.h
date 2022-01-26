#pragma once
#include <GL/glew.h>
#include <Gengine/GLSLProgram.h>
#include <Gengine/spriteBatch.h>
#include <Gengine/debugRenderer.h>
#include <Gengine/camera2D.h>
#include <Gengine/timing.h>
#include <Gengine/meshBatch.h>
#include <Box2D\Box2D.h>
#include <math.h>
#include "userData.h"
#include "tileBatch.h"
#include "luaHandler.h"
#include "largeTileBody.h"

class lightQueryCallback : public b2QueryCallback {
public:
	std::vector<b2Body*> foundBodies;

	bool ReportFixture(b2Fixture* fixture) {
		if (fixture->GetUserData() != nullptr) {
			if (static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILE || static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::SOLID_BODY) {
				for (int i = 0; i < foundBodies.size(); i++) {
					if (fixture->GetBody() == foundBodies[i]) {
						return true;
					}
				}
				foundBodies.push_back(fixture->GetBody());
			}
		}
		return true;
	}
};

class lightRayCastCallback : public b2RayCastCallback {
public:
	float closestFraction;

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		if (fixture->GetUserData() != nullptr) {
			if (static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILE || static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::SOLID_BODY) {
				if (closestFraction >= fraction) {
					closestFraction = fraction;
					return fraction;
				}
			}
		}
		return 1;
	}
};

//Skips the first fixture hit and goes to the second one
class lightRayCastCallbackInside : public b2RayCastCallback {
public:
	float secondclosestFraction; // = 1
	float firstClosestFraction; // = 0

	float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float32 fraction) {
		if (fixture->GetUserData() != nullptr) {
			if (static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::TILE || static_cast<userData*>(fixture->GetUserData())->type == gameObjectType::SOLID_BODY) {
				if (firstClosestFraction == 0) {
					firstClosestFraction = fraction;
				}
				else if (fraction < firstClosestFraction && fraction < secondclosestFraction) {
					secondclosestFraction = firstClosestFraction;
					firstClosestFraction = fraction;
				}
				else if (fraction < secondclosestFraction) {
					secondclosestFraction = fraction;
				}


			}
		}
		return 1;
	}
};

struct light {
	void draw(gengine::spriteBatch* spriteBatch, gengine::meshBatch* meshBatch, gengine::camera2D* camera, GLint intensityLoc, GLint coneAngleLoc, GLint lightDirectionLoc, b2World* world, bool modified) {
		if (camera->isInCameraView(*_position, glm::vec2(radius, radius))) {
			if (_lastPosition != *_position + _offset) {
				_pointsVector = getLightMeshPoints(world);
			}
			else if (modified) {
				_pointsVector = getLightMeshPoints(world);
			}
			else if (_pointsVector.empty()) {
				_pointsVector = getLightMeshPoints(world);
			}

			if (!_pointsVector.empty()) {
				meshBatch->draw(&_pointsVector, glm::vec2(_position->x + _offset.x, _position->y + _offset.y), glm::vec2(radius * 2, radius * 2), color);
			}
		}
		_lastPosition = *_position + _offset;
	}
	
	void debugDraw(gengine::debugRenderer* debugRenderer, b2World* world, gengine::camera2D* camera) {
		if (camera->isInCameraView(*_position, glm::vec2(radius * 2, radius * 2))) {
			std::vector<glm::vec2> pointVector = getLightMeshPoints(world);
			for (int i = 0; i < pointVector.size(); i++) {
				debugRenderer->drawLine(glm::vec2(_position->x + _offset.x, _position->y + _offset.y), pointVector[i], gengine::color(255, 0, 0, 255));
			}
		}
	}

	std::vector<glm::vec2> getLightMeshPoints(b2World* world) {
		b2Vec2 lightCenterPosition(_position->x + _offset.x, _position->y + _offset.y);
		lightQueryCallback queryCallback;

		b2AABB aabb;
		aabb.lowerBound = b2Vec2(lightCenterPosition.x - radius, lightCenterPosition.y - radius);
		aabb.upperBound = b2Vec2(lightCenterPosition.x + radius, lightCenterPosition.y + radius);
		world->QueryAABB(&queryCallback, aabb);

		std::vector<b2Body*> bodiesInRange;
		bodiesInRange = queryCallback.foundBodies;

		bool insidePlacableItem = false;

		//Loops to see if the light is inside a placable item
		for (int i = 0; i < bodiesInRange.size();i++) {
			if (static_cast<userData*>(bodiesInRange[i]->GetFixtureList()->GetUserData())->type == gameObjectType::SOLID_BODY) {
				largeTileBody* currenttileBatch = static_cast<largeTileBody*>(static_cast<userData*>(bodiesInRange[i]->GetFixtureList()->GetUserData())->dataUser);
				glm::vec2 dimensions = currenttileBatch->getDimensions();
				b2Vec2 bodyPosition = bodiesInRange[i]->GetPosition();

				if (lightCenterPosition.x > bodyPosition.x + dimensions.x / 2 || lightCenterPosition.x < bodyPosition.x - dimensions.x / 2
					|| lightCenterPosition.y > bodyPosition.y + dimensions.y / 2 || lightCenterPosition.y < bodyPosition.y - dimensions.y / 2) {

				}
				else {
					bodiesInRange[i] = bodiesInRange.back();
					bodiesInRange.pop_back();
					insidePlacableItem = true;
					break;
				}
			}
		}

		std::vector<glm::vec2> meshPoints;
		glm::vec2 currentVertex;

		lightRayCastCallback rayCastCallback;
		lightRayCastCallbackInside rayCastCallbackInside;
		float maxLength = radius;

		if (!insidePlacableItem) {
			//Push the 4 corners of the light
			b2Vec2 rayCastedPoint = b2Vec2(lightCenterPosition.x - radius, lightCenterPosition.y - radius);
			meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x + radius, lightCenterPosition.y - radius);
			meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x - radius, lightCenterPosition.y + radius);
			meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x + radius, lightCenterPosition.y + radius);
			meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
		}
		else {
			//Push the 4 corners of the light
			b2Vec2 rayCastedPoint = b2Vec2(lightCenterPosition.x - radius, lightCenterPosition.y - radius);
			meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x + radius, lightCenterPosition.y - radius);
			meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x - radius, lightCenterPosition.y + radius);
			meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));

			rayCastedPoint = b2Vec2(lightCenterPosition.x + radius, lightCenterPosition.y + radius);
			meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
		}


		for (b2Body* it : bodiesInRange) {
			if (static_cast<userData*>(it->GetFixtureList()->GetUserData())->type == gameObjectType::SOLID_BODY) {
				largeTileBody* currenttileBatch = static_cast<largeTileBody*>(static_cast<userData*>(it->GetFixtureList()->GetUserData())->dataUser);
				glm::vec2 dimensions = currenttileBatch->getDimensions();
				b2Vec2 bodyPosition = it->GetPosition();

				if (!insidePlacableItem) {
					//Vertex 1
					b2Vec2 rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
					b2Vec2 toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.005f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 2
					rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.005f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 3
					rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.02f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 4
					rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.02f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
				}
				else {
					//Vertex 1
					b2Vec2 rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
					b2Vec2 toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.005f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 2
					rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.005f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 3
					rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.02f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

					//Vertex 4
					rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
					toRotate = rayCastedPoint - lightCenterPosition;
					toRotate = rotateVector(0.01f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					toRotate = rotateVector(-0.02f, toRotate);
					meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
				}
			}
			else if (static_cast<userData*>(it->GetFixtureList()->GetUserData())->type == gameObjectType::TILE) {
				tileBatch* currenttileBatch = static_cast<tileBatch*>(static_cast<userData*>(it->GetFixtureList()->GetUserData())->dataUser);
				glm::vec2 dimensions = currenttileBatch->getDimensions();
				b2Vec2 bodyPosition = it->GetPosition();

				if (lightCenterPosition.x > bodyPosition.x + dimensions.x / 2 || lightCenterPosition.x < bodyPosition.x - dimensions.x / 2
					|| lightCenterPosition.y > bodyPosition.y + dimensions.y / 2 || lightCenterPosition.y < bodyPosition.y - dimensions.y / 2) {
					if (!insidePlacableItem) {
						//Vertex 1
						b2Vec2 rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
						b2Vec2 toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.005f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 2
						rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.005f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 3
						rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.02f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 4
						rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.02f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallback, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					}
					else {
						//Vertex 1
						b2Vec2 rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
						b2Vec2 toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.005f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 2
						rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y - dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.005f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 3
						rayCastedPoint = b2Vec2(bodyPosition.x - dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.02f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));

						//Vertex 4
						rayCastedPoint = b2Vec2(bodyPosition.x + dimensions.x / 2, bodyPosition.y + dimensions.y / 2);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, rayCastedPoint, maxLength));
						toRotate = rayCastedPoint - lightCenterPosition;
						toRotate = rotateVector(0.01f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
						toRotate = rotateVector(-0.02f, toRotate);
						meshPoints.push_back(rayCast(world, &rayCastCallbackInside, lightCenterPosition, lightCenterPosition + toRotate, maxLength));
					}
					
				}
				else {
					meshPoints.clear();
					break;
				}

			
			}
		}

		return meshPoints;
	}

	b2Vec2 rotateVector(float angleToRotate, b2Vec2& toRotate) {
		float bearingRadians = atan2(toRotate.y, toRotate.x); // get bearing in radians
		bearingRadians += angleToRotate;
		b2Vec2 returned;
		returned.x = cos(bearingRadians) * toRotate.Length();
		returned.y = sin(bearingRadians) * toRotate.Length();
		return returned;
	}

	glm::vec2 rayCast(b2World* world, lightRayCastCallback* rayCastCallback, b2Vec2& point1, b2Vec2& point2, float maxLength) {
		b2Vec2 direction = b2Vec2(point2 - point1);		
		float slope = direction.y / direction.x;

		b2Vec2 intersectionPoint;
		if (abs(direction.x) > abs(direction.y)) {
			intersectionPoint.x = maxLength;
			intersectionPoint.y = maxLength * slope;
		}
		else if(abs(direction.x) < abs(direction.y)) {
			intersectionPoint.x = maxLength / slope;
			intersectionPoint.y = maxLength;
		}
		else {
			intersectionPoint.x = maxLength;
			intersectionPoint.y = maxLength;
		}

		if (direction.x > 0) {
			if (intersectionPoint.x < 0) {
				intersectionPoint.x *= -1;
			}
		}
		else if (direction.x < 0) {
			if (intersectionPoint.x > 0) {
				intersectionPoint.x *= -1;
			}
		}
		if (direction.y > 0) {
			if (intersectionPoint.y < 0) {
				intersectionPoint.y *= -1;
			}
		}
		else if (direction.y < 0) {
			if (intersectionPoint.y > 0) {
				intersectionPoint.y *= -1;
			}
		}

		rayCastCallback->closestFraction = 1;

		world->RayCast(rayCastCallback, point1, intersectionPoint + point1);
		return glm::vec2(point1.x + rayCastCallback->closestFraction * (intersectionPoint.x), point1.y + rayCastCallback->closestFraction * (intersectionPoint.y));
	}

	glm::vec2 rayCast(b2World* world, lightRayCastCallbackInside* rayCastCallback, b2Vec2& point1, b2Vec2& point2, float maxLength) {
		b2Vec2 direction = b2Vec2(point2 - point1);
		float slope = direction.y / direction.x;

		b2Vec2 intersectionPoint;
		if (abs(direction.x) > abs(direction.y)) {
			intersectionPoint.x = maxLength;
			intersectionPoint.y = maxLength * slope;
		}
		else if (abs(direction.x) < abs(direction.y)) {
			intersectionPoint.x = maxLength / slope;
			intersectionPoint.y = maxLength;
		}
		else {
			intersectionPoint.x = maxLength;
			intersectionPoint.y = maxLength;
		}

		if (direction.x > 0) {
			if (intersectionPoint.x < 0) {
				intersectionPoint.x *= -1;
			}
		}
		else if (direction.x < 0) {
			if (intersectionPoint.x > 0) {
				intersectionPoint.x *= -1;
			}
		}
		if (direction.y > 0) {
			if (intersectionPoint.y < 0) {
				intersectionPoint.y *= -1;
			}
		}
		else if (direction.y < 0) {
			if (intersectionPoint.y > 0) {
				intersectionPoint.y *= -1;
			}
		}

		rayCastCallback->firstClosestFraction = 0;
		rayCastCallback->secondclosestFraction = 1;

		world->RayCast(rayCastCallback, point1, intersectionPoint + point1);
		return glm::vec2(point1.x + rayCastCallback->secondclosestFraction * (intersectionPoint.x), point1.y + rayCastCallback->secondclosestFraction * (intersectionPoint.y));
	}

	void setPositionSetter(glm::vec2* position) {
		_position = position;
	}
	void setOffset(glm::vec2 offset) {
		_offset = offset;
	}
	void setDirection(glm::vec2 direction){
		lightDirection = direction;
	}
	void setTypeID(int typeID) {
		_typeID = typeID;
	}
	void setRadius(float radius, b2World* world) {
		this->radius = radius;
	}
	int getTypeID() {
		return _typeID;
	}
	float getRadius() {
		return radius;
	}
	float getIntensity() {
		return intensity;
	}

	void calculateTypeID() {
		_typeID = intensity * intensity + ((color.r + color.g + color.b) / 3);
	}

	void calculatePoints(b2World* world){
		_pointsVector = getLightMeshPoints(world);
	}

	gengine::color getColor() {
		return color;
	}

	bool isTheSame(light& lightToCompare) {
		if (lightToCompare.intensity == intensity && lightToCompare.coneAngle == coneAngle && lightToCompare.lightDirection.x == lightDirection.x && lightToCompare.lightDirection.y == lightDirection.y)
		{
			return true;
		}
		else {
			return false;
		}
	}

	gengine::color color = gengine::color(255,255,255,255);
	float intensity = 4.5;
	glm::vec2 lightDirection = glm::vec2(1,0);
	float coneAngle = 180; // The half-angle of the arc of the cone in a full circle
private:
	float radius = 1;
	glm::vec2* _position = nullptr;
	glm::vec2 _lastPosition = glm::vec2(-500, -500);
	glm::vec2 _offset = glm::vec2(0,0);
	int _typeID; //Will hold the type that matches all lights like it so they can be sorted and be drawn together
	std::vector<glm::vec2> _pointsVector;
};

class lightSystem
{
public:
	lightSystem();
	~lightSystem();

	void initGLSLPrograms();
	void useProgram();
	void unuseProgram();

	void renderToRenderBuffer();
	void renderToShadowBuffer();
	void renderToTileBuffer();
	void copyToShadowBuffer();
	void copyToShadowBufferAndClear();
	void copyToDownsamplingBuffer();
	void renderToBlurBuffer();
	void unbindBuffer();
	void downSampleShadowBuffer(gengine::spriteBatch* spriteBatch);
	void renderProduct(gengine::spriteBatch* spriteBatch, gengine::GLSLProgram* textureProgram);
	void addAmbiantLightToShadowMap(gengine::spriteBatch* spriteBatch, gengine::color ambiantLightColor, gengine::GLSLProgram* textureProgram, gengine::camera2D* camera);
	void blurShadows(gengine::spriteBatch* spriteBatch);
	void blurDownsampledTexture(gengine::spriteBatch* spriteBatch);
	GLint getUniformLocation(std::string location);
	GLint lightApplyGetUniformLocation(std::string location);
	GLint ambiantLightGetUniformLocation(std::string location);

	void createBuffers();

private:
	void _createFrameBuffer();
	void _createShadowMap();
	void _createBlurBuffer();
	void _createTileBuffer();
	void _createDownsamplingBuffer();
	
	const glm::vec2 FRAME_BUFFER_RESOLUTION = glm::vec2(1080, 720);
	const glm::vec2 SHADOW_MAP_RESOLUTION = glm::vec2(512, 512);
	const glm::vec2 BLUR_BUFFER_RESOLUTION = glm::vec2(512, 512);
	const glm::vec2 TILE_BUFFER_RESOLUTION = glm::vec2(512, 512);
	const glm::vec2 DOWNSAMPLING_BUFFER_RESOLUTION = glm::vec2(192, 192);

	gengine::GLSLProgram _lightProgram;
	gengine::GLSLProgram _lightApplyProgram; //Also takes care of the vertical blur
	gengine::GLSLProgram _horizontalBlurProgram;
	gengine::GLSLProgram _verticalBlurProgram;
	gengine::GLSLProgram _largeHorizontalBlurProgram;
	gengine::GLSLProgram _largeVerticalBlurProgram;
	gengine::GLSLProgram _ambiantLightProgram;
	gengine::GLSLProgram _copyShader;


	GLuint frameBufferTexture;
	GLuint shadowMap;
	GLuint horizontalBlurTexture;
	GLuint tileTexture;
	GLuint downsamplingTexture;

	GLuint colorFrameBuffer;
	GLuint shadowFrameBuffer;
	GLuint horizontalFrameBuffer;
	GLuint tileFrameBuffer;
	GLuint downsamplingFrameBuffer;

	GLint colorSamplerLoc;
	GLint shadowSamplerLoc;
	GLint tileSamplerLoc;
	GLint shadowSampler2Loc;
	GLint hBlurSamplerLoc;
	GLint vBlurSamplerLoc;
};

