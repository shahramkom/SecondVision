/*
 *
 *	Compact JSON format parsing lib (native cross-platform c++)
 *
 *	Copyright (C) 2013 Victor Laskin (victor.laskin@gmail.com)
 *	Details: http://vitiy.info/?p=102
 *
 *	Redistribution and use in source and binary forms, with or without
 *	modification, are permitted provided that the following conditions
 *	are met:
 *	1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  3. The names of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 *	THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
 *	OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *	ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 *	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 *	IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *	IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "MVJSON.h"

namespace JSON {


	MVJSONReader::MVJSONReader(const string& source) {
		root = nullptr;
		if (source == "") return;


		if (source[0] == '{')
			root = parse(source);
		if (source[0] == '[')
			root = parse("{\"root\":" + source + "}");
	}

	MVJSONReader::~MVJSONReader() {
		if (root != NULL)
			delete root;
	}

	MVJSONNode::~MVJSONNode() {
		if (values.size() > 0)
			for (int i = 0; i < values.size(); i++)
				delete values.at(i);
	}



	MVJSONNode * MVJSONReader::parse(string text)
	{
		string s = trim(text);
		if (s.length() < 2) return NULL;

		// object
		if ((s[0] == '{') && (s[s.length() - 1] == '}'))
		{
			// erase last and first symbols
			s.erase(0, 1);
			s.erase(s.length() - 1, 1);

			vector<string> parts;
			splitList(s, parts);

			MVJSONNode* node = new MVJSONNode();

			for (int i = 0; i < parts.size(); i++)
				node->values.push_back(parseValue(parts.at(i), false));

			return node;
		}

		return NULL;
	}




	MVJSONValue* MVJSONReader::parseValue(string text, bool hasNoName)
	{
		string key;
		string s;
		splitInHalf(text, ":", key, s);
		key = trim(key);
		s = trim(s);
		if (key.length() > 2)
		{
			// strip "
			key.erase(0, 1);
			key.erase(key.length() - 1, 1);
		}

		if (hasNoName)
		{
			s = text;
			key = "";
		}


		if (s == "false") // bool
			return new MVJSONValue(key, false);

		if (s == "true")  // bool
			return new MVJSONValue(key, true);

		if (s == "null") // null
			return new MVJSONValue(key, MVJSON_TYPE_NULL);

		char first = s[0];

		if (first == '"') // string
			return new MVJSONValue(key, s.substr(1, s.length() - 2));

		if (first == '{') // object
			return new MVJSONValue(key, parse(s));

		if (first == '[') // array
		{
			s.erase(0, 1);
			s.erase(s.length() - 1, 1);

			MVJSONValue* val = new MVJSONValue(key, MVJSON_TYPE_ARRAY);

			if (s != "")
			{
				vector<string> parts;
				splitList(s, parts);
				for (int i = 0; i < parts.size(); i++)
					val->arrayValue.push_back(parseValue(parts.at(i), true));
			}
			return val;
		}

		// else its number!
		if (s.find(".") == string::npos)
			return new MVJSONValue(key, s, stoll(s));
		else
			return new MVJSONValue(key, s, stod(s));

		return NULL;
	}

	MVJSONValue::~MVJSONValue()
	{
		if (objValue != NULL)
			delete objValue;
		if (arrayValue.size() > 0)
			for (int i = 0; i < arrayValue.size(); i++)
				delete arrayValue.at(i);
	}

	void MVJSONValue::init(MVJSON_TYPE valueType)
	{
		this->valueType = valueType;
		objValue = NULL;
		name = "";
	}

	MVJSONValue::MVJSONValue(const string & name, MVJSON_TYPE valueType)
	{
		init(valueType);
		this->name = name;
	}

	MVJSONValue::MVJSONValue(const string & name, bool value)
	{
		init(MVJSON_TYPE_BOOL);
		this->name = name;
		boolValue = value;
	}

	MVJSONValue::MVJSONValue(const string & name, const string & value)
	{
		init(MVJSON_TYPE_STRING);
		this->name = name;

		stringValue = value;

		// here we switch back special chars
		//	\"	\\	\/	\b	\f	\n	\r	\t	\u four-hex-digits
		replace(stringValue, "\\\"", "\"");
		replace(stringValue, "\\\\", "\\");
		replace(stringValue, "\\/", "/");
		replace(stringValue, "\\b", "\b");
		replace(stringValue, "\\f", "\f");
		replace(stringValue, "\\n", "\n");
		replace(stringValue, "\\r", "\r");
		replace(stringValue, "\\t", "\t");

		// TODO - \u four-hex-digits
		// SS::replace(stringValue, "\\\\", "\\");

	}

	MVJSONValue::MVJSONValue(const string & name, const string & source, long long value)
	{
		init(MVJSON_TYPE_INT);
		this->name = name;
		stringValue = source;
		intValue = value;
	}

	MVJSONValue::MVJSONValue(const string & name, const string & source, double value)
	{
		init(MVJSON_TYPE_DOUBLE);
		this->name = name;
		stringValue = source;
		doubleValue = value;
	}

	MVJSONValue::MVJSONValue(const string & name, MVJSONNode * value)
	{
		init(MVJSON_TYPE_OBJECT);
		this->name = name;
		objValue = value;
	}

	bool MVJSONNode::hasField(const string & name)
	{
		if (values.size() == 0) return false;
		for (int i = 0; i < values.size(); i++)
			if (values.at(i)->name == name)
				return true;
		return false;
	}

	MVJSONValue * MVJSONNode::getField(const string & name)
	{
		if (values.size() == 0) return NULL;
		for (int i = 0; i < values.size(); i++)
			if (values.at(i)->name == name)
				return values.at(i);
		return NULL;
	}

	MVJSONValue * MVJSONValue::field(const string & name) {
		return objValue->getField(name);
	}




	double MVJSONNode::getFieldDouble(const string & name)
	{
		MVJSONValue* value = getField(name);
		if (value == NULL) return 0;
		if (value->valueType == MVJSON_TYPE_INT) return value->intValue;
		if (value->valueType == MVJSON_TYPE_DOUBLE) return value->doubleValue;
		return 0;
	}

	int MVJSONNode::getFieldInt(const string & name)
	{
		MVJSONValue* value = getField(name);
		if (value == NULL) return 0;
		if (value->valueType == MVJSON_TYPE_INT) return (int)value->intValue;
		return 0;
	}

	long long MVJSONNode::getFieldLongLong(const string & name)
	{
		MVJSONValue* value = getField(name);
		if (value == NULL) return 0;
		if (value->valueType == MVJSON_TYPE_INT) return value->intValue;
		return 0;
	}

	string MVJSONNode::getFieldString(const string & name)
	{
		MVJSONValue* value = getField(name);
		if (value == NULL) return "";
		if (value->valueType == MVJSON_TYPE_STRING) return value->stringValue;
		if (value->valueType == MVJSON_TYPE_DOUBLE) return value->stringValue;
		if (value->valueType == MVJSON_TYPE_INT) return value->stringValue;
		return "";
	}

	bool MVJSONNode::getFieldBool(const string & name)
	{
		MVJSONValue* value = getField(name);
		if (value == NULL) return false;
		if (value->valueType == MVJSON_TYPE_INT) return (bool)value->intValue;
		if (value->valueType == MVJSON_TYPE_BOOL) return value->boolValue;
		return false;
	}



	double MVJSONValue::getFieldDouble(const string & name)
	{
		if (objValue == NULL) return 0;
		return objValue->getFieldDouble(name);
	}

	int MVJSONValue::getFieldInt(const string & name)
	{
		if (objValue == NULL) return 0;
		return objValue->getFieldInt(name);
	}

	long long  MVJSONValue::getFieldLongLong(const string & name)
	{
		if (objValue == NULL) return 0;
		return objValue->getFieldLongLong(name);
	}

	string MVJSONValue::getFieldString(const string & name)
	{
		if (objValue == NULL) return "";
		return objValue->getFieldString(name);
	}

	bool MVJSONValue::getFieldBool(const string & name)
	{
		if (objValue == NULL) return false;
		return objValue->getFieldBool(name);
	}





	// -------------------- writer -------------------------->

	MVJSONWriter::MVJSONWriter()
	{
		result = "";
		depth = 0;
		counts.resize(50);
		std::fill(counts.begin(), counts.end(), 0);
	}

} /* namespace F2 */
