#include "Gui.h"

namespace ofxSerialize
{
	//--------------------------------------------------------------
	static Gui::WindowOpen windowOpen;

	//--------------------------------------------------------------
	Gui::Settings::Settings()
		: windowPos(kGuiMargin, kGuiMargin)
		, windowSize(ofVec2f::zero())
		, windowBlock(false)
		, mouseOverGui(false)
	{}

	//--------------------------------------------------------------
	void Gui::SetNextWindow(Settings & settings)
	{
		settings.windowSize.x = 0;
		settings.windowPos.y += settings.windowSize.y + kGuiMargin;
	}

	//--------------------------------------------------------------
	bool Gui::BeginWindow(ofParameter<bool> & parameter, Settings & settings, bool collapse)
	{
		// Reference this ofParameter until EndWindow().
		windowOpen.parameter = dynamic_pointer_cast<ofParameter<bool>>(parameter.newReference());
		windowOpen.value = parameter.get();

		auto result = Gui::BeginWindow(parameter.getName(), settings, collapse, &windowOpen.value);
		parameter = windowOpen.value;
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::BeginWindow(const string & name, Settings & settings, bool collapse, bool * opened)
	{
		if (settings.windowBlock)
		{
			ofLogWarning("Gui::BeginWindow") << "Already inside a window block!";
			return false;
		}

		settings.windowBlock = true;

		ImGui::SetNextWindowPos(settings.windowPos, ImGuiSetCond_Appearing);
		ImGui::SetNextWindowSize(settings.windowSize, ImGuiSetCond_Appearing);
		ImGui::SetNextWindowCollapsed(collapse, ImGuiSetCond_Appearing);
		return ImGui::Begin(name.c_str(), opened, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize | (collapse ? 0 : ImGuiWindowFlags_NoCollapse));
	}

	//--------------------------------------------------------------
	void Gui::EndWindow(Settings & settings)
	{
		if (!settings.windowBlock)
		{
			ofLogWarning("Gui::EndWindow") << "Not inside a window block!";
			return;
		}

		settings.windowBlock = false;

		settings.windowPos = ImGui::GetWindowPos();
		settings.windowSize = ImGui::GetWindowSize();
		ImGui::End();

		// Unlink the referenced ofParameter.
		windowOpen.parameter.reset();

		const auto bounds = ofRectangle(settings.windowPos, settings.windowSize.x, settings.windowSize.y);
		settings.mouseOverGui |= bounds.inside(ofGetMouseX(), ofGetMouseY());
	}

	//--------------------------------------------------------------
	void Gui::AddGroup(ofParameterGroup & group, Settings & settings)
	{
		bool prevWindowBlock = settings.windowBlock;
		if (settings.windowBlock)
		{
			if (!ImGui::CollapsingHeader(group.getName().c_str(), nullptr, true))
			{
				return;
			}
		}
		else
		{
			Gui::BeginWindow(group.getName().c_str(), settings);
		}

		for (auto parameter : group)
		{
			// Group.
			auto parameterGroup = dynamic_pointer_cast<ofParameterGroup>(parameter);
			if (parameterGroup)
			{
				// Recurse through contents.
				Gui::AddGroup(*parameterGroup, settings);
				continue;
			}

			// Parameter, try everything we know how to handle.
			auto parameterVec2f = dynamic_pointer_cast<ofParameter<ofVec2f>>(parameter);
			if (parameterVec2f)
			{
				Gui::AddParameter(*parameterVec2f);
				continue;
			}
			auto parameterVec3f = dynamic_pointer_cast<ofParameter<ofVec3f>>(parameter);
			if (parameterVec3f)
			{
				Gui::AddParameter(*parameterVec3f);
				continue;
			}
			auto parameterVec4f = dynamic_pointer_cast<ofParameter<ofVec4f>>(parameter);
			if (parameterVec4f)
			{
				Gui::AddParameter(*parameterVec4f);
				continue;
			}
			auto parameterFloatColor = dynamic_pointer_cast<ofParameter<ofFloatColor>>(parameter);
			if (parameterFloatColor)
			{
				Gui::AddParameter(*parameterFloatColor);
				continue;
			}
			auto parameterFloat = dynamic_pointer_cast<ofParameter<float>>(parameter);
			if (parameterFloat)
			{
				Gui::AddParameter(*parameterFloat);
				continue;
			}
			auto parameterInt = dynamic_pointer_cast<ofParameter<int>>(parameter);
			if (parameterInt)
			{
				Gui::AddParameter(*parameterInt);
				continue;
			}
			auto parameterBool = dynamic_pointer_cast<ofParameter<bool>>(parameter);
			if (parameterBool)
			{
				Gui::AddParameter(*parameterBool);
				continue;
			}

			ofLogWarning("Gui::AddGroup") << "Could not create GUI element for parameter " << parameter->getName();
		}

		// Only end window if we created it.
		if (settings.windowBlock && !prevWindowBlock)
		{
			Gui::EndWindow(settings);
		}
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::tvec2<int>> & parameter)
	{
		static glm::tvec2<int> tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderInt2(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::tvec3<int>> & parameter)
	{
		static glm::tvec3<int> tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderInt3(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::tvec4<int>> & parameter)
	{
		static glm::tvec4<int> tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderInt4(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::vec2> & parameter)
	{
		static glm::vec2 tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat2(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::vec3> & parameter)
	{
		static glm::vec3 tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat3(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<glm::vec4> & parameter)
	{
		static glm::vec4 tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat4(parameter.getName().c_str(), glm::value_ptr(tmpRef), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<ofVec2f> & parameter)
	{
		static ofVec2f tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat2(parameter.getName().c_str(), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<ofVec3f> & parameter)
	{
		static ofVec3f tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat3(parameter.getName().c_str(), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<ofVec4f> & parameter)
	{
		static ofVec4f tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::SliderFloat4(parameter.getName().c_str(), tmpRef.getPtr(), parameter.getMin().x, parameter.getMax().x))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddParameter(ofParameter<ofFloatColor> & parameter)
	{
		static ofFloatColor tmpRef;
		
		tmpRef = parameter.get();
		if (ImGui::ColorEdit4(parameter.getName().c_str(), &tmpRef.r))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	template<typename ParameterType>
	bool Gui::AddParameter(ofParameter<ParameterType> & parameter)
	{
		static ParameterType tmpRef;

		const auto & info = typeid(ParameterType);
		tmpRef = parameter.get();
		if (info == typeid(float))
		{
			if (ImGui::SliderFloat(parameter.getName().c_str(), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
			{
				parameter.set(tmpRef);
				return true;
			}
			return false;
		}
		if (info == typeid(int))
		{
			if (ImGui::SliderInt(parameter.getName().c_str(), (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
			{
				parameter.set(tmpRef);
				return true;
			}
			return false;
		}
		if (info == typeid(bool))
		{
			if (ImGui::Checkbox(parameter.getName().c_str(), (bool *)&tmpRef))
			{
				parameter.set(tmpRef);
				return true;
			}
			return false;
		}

		ofLogWarning(__FUNCTION__) << "Could not create GUI element for type " << info.name();
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddRadio(ofParameter<int> & parameter, vector<string> labels, int columns)
	{
		static int tmpRef;

		tmpRef = parameter.get();
		auto result = false;
		ImGui::Columns(columns);
		for (int i = 0; i < labels.size(); ++i)
		{
			result |= ImGui::RadioButton(labels[i].c_str(), &tmpRef, i); ImGui::NextColumn();
		}
		ImGui::Columns(1);
		parameter.set(tmpRef);
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddRange(const string & name, ofParameter<float> & parameterMin, ofParameter<float> & parameterMax, float speed)
	{
		static float tmpRefMin;
		static float tmpRefMax;

		tmpRefMin = parameterMin.get();
		tmpRefMax = parameterMax.get();
		if (ImGui::DragFloatRange2(name.c_str(), &tmpRefMin, &tmpRefMax, speed, parameterMin.getMin(), parameterMax.getMax()))
		{
			parameterMin.set(tmpRefMin);
			parameterMax.set(tmpRefMax);
			return true;
		}
		return false;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::tvec2<int>> & values, int minValue, int maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderInt2(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::tvec3<int>> & values, int minValue, int maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderInt3(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::tvec4<int>> & values, int minValue, int maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderInt4(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::vec2> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat2(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::vec3> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat3(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<glm::vec4> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat4(iname.c_str(), glm::value_ptr(values[i]), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<ofVec2f> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat2(iname.c_str(), values[i].getPtr(), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<ofVec3f> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat3(iname.c_str(), values[i].getPtr(), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	bool Gui::AddValues(const string & name, vector<ofVec4f> & values, float minValue, float maxValue)
	{
		auto result = false;
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			result |= ImGui::SliderFloat4(iname.c_str(), values[i].getPtr(), minValue, maxValue);
		}
		return result;
	}

	//--------------------------------------------------------------
	template<typename DataType>
	bool Gui::AddValues(const string & name, vector<DataType> & values, DataType minValue, DataType maxValue)
	{
		auto result = false;

		const auto & info = typeid(DataType);
		for (int i = 0; i < values.size(); ++i)
		{
			const auto iname = name + " " + ofToString(i);
			if (info == typeid(float))
			{
				result |= ImGui::SliderFloat(iname.c_str(), *values[i], minValue, maxValue);
			}
			else if (info == typeid(int))
			{
				result |= ImGui::SliderInt(iname.c_str(), *values[i], minValue, maxValue);
			}
			else if (info == typeid(bool))
			{
				result |= ImGui::Checkbox(iname.c_str(), *values[i]);
			}
			else
			{
				ofLogWarning("Gui::AddValues") << "Could not create GUI element for type " << info.name();
				return false;
			}
		}

		return result;
	}

	//--------------------------------------------------------------
	void Gui::AddImage(ofBaseHasTexture & hasTexture, const ofVec2f & size)
	{
		Gui::AddImage(hasTexture.getTexture(), size);
	}

	//--------------------------------------------------------------
	void Gui::AddImage(ofTexture & texture, const ofVec2f & size)
	{
		ImTextureID textureID = (ImTextureID)(uintptr_t)texture.texData.textureID;
		ImGui::Image(textureID, size);
	}
}
