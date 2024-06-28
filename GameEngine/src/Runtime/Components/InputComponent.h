#pragma once
#include <set>

#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>

#include "Input/InputKeys.h"

struct InputComponent
{
	std::set<InputKey> inputKeys;
	std::set<InputMouse> inputMouseButtons;

	InputComponent()
	{
	}

	InputComponent(ryml::NodeRef node)
	{
		if(node.has_child("inputKeys"))
		{

			auto child = node["inputKeys"].first_child();
			for(int i = 0; i < node["inputKeys"].num_children(); i++)
			{
				int key = 0;
				child >> key;
				inputKeys.insert((InputKey)key);
				child = child.next_sibling();
			}
		}

		if(node.has_child("inputMouseButtons"))
		{
			auto child = node["inputMouseButtons"].first_child();
			for(int i = 0; i < node["inputMouseButtons"].num_children(); i++)
			{
				int key = 0;
				child >> key;
				inputMouseButtons.insert((InputMouse)key);
				child = child.next_sibling();
			}
		}
	}

	void Serialize(ryml::NodeRef node)
	{
		node |= ryml::MAP;
		node["inputKeys"] |= ryml::SEQ;
		for(auto& key : inputKeys)
		{
			node["inputKeys"].append_child() << (int)key;
		}
		node["inputMouseButtons"] |= ryml::SEQ;
		for(auto& key : inputMouseButtons)
		{
			node["inputMouseButtons"].append_child() << (int)key;
		}
	}
};