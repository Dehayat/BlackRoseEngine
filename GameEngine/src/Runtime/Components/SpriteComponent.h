#pragma once
#include <string>
#include <memory>
#include<optional>


#include <SDL2/SDL_rect.h>
#include <ryml/ryml.hpp>
#include <glm/glm.hpp>

#include "Reflection/Reflection.h"

using namespace glm;

const SDL_Rect DEFAULT_RECT = SDL_Rect{-1};

class SpriteComponent
{
public:
	std::string sprite;
	glm::vec4 color;

	SDL_Rect sourceRectData;
	SDL_Rect* sourceRect;
	SDL_FRect destRect;
	int layer;

	SpriteComponent(std::string sprite = "block", int layer = 0, vec4 color = vec4(1, 1, 1, 1))
	{
		this->sprite = sprite;
		this->layer = layer;
		this->color = color;
		sourceRectData = DEFAULT_RECT;
		sourceRect = nullptr;
		destRect = SDL_FRect();
	}
	SpriteComponent(ryml::NodeRef node)
	{
		this->sprite = "";
		this->color = vec4(1, 1, 1, 1);
		this->layer = 0;
		sourceRectData = DEFAULT_RECT;
		sourceRect = nullptr;
		destRect = SDL_FRect();
		if(node.is_map())
		{
			if(node.has_child("sprite"))
			{
				node["sprite"] >> this->sprite;
			}
			if(node.has_child("layer"))
			{
				node["layer"] >> this->layer;
			}
			if(node.has_child("color"))
			{
				auto c = vec4();
				node["color"][0] >> c.r;
				node["color"][1] >> c.g;
				node["color"][2] >> c.b;
				node["color"][3] >> c.a;
				this->color = c;
			}
		}
	}
	SpriteComponent& operator=(const SpriteComponent& other)
	{
		this->sprite = other.sprite;
		this->layer = other.layer;
		this->color = other.color;
		this->sourceRectData = other.sourceRectData;
		this->destRect = other.destRect;
		if(other.sourceRect != nullptr)
		{
			this->sourceRect = &this->sourceRectData;
		} else
		{
			this->sourceRect = nullptr;
		}
		return *this;
	}
	SpriteComponent(const SpriteComponent& other)
	{
		this->sprite = other.sprite;
		this->layer = other.layer;
		this->color = other.color;
		this->sourceRectData = other.sourceRectData;
		this->destRect = other.destRect;
		if(other.sourceRect != nullptr)
		{
			this->sourceRect = &this->sourceRectData;
		} else
		{
			this->sourceRect = nullptr;
		}
	}

	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["sprite"] << sprite;
		node["layer"] << layer;
		node["color"] |= ryml::SEQ;
		node["color"].append_child() << color.x;
		node["color"].append_child() << color.y;
		node["color"].append_child() << color.z;
		node["color"].append_child() << color.w;
	}
	ROSE_EXPOSE_VARS(SpriteComponent, (sprite)(layer)(color))
};