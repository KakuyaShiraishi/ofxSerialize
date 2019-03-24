#pragma once

#include "ofxSerialize/Gui.h"
#include "ofxSerialize/Serializer.h"

#define PARAM_DECLARE(NAME, ...) bool paramDeclareConstructor \
{ [this] { this->setName(NAME), this->add(__VA_ARGS__); return true; }() };
