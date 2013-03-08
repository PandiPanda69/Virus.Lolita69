#include "sandbox.h"

namespace antivirus
{
	/**
	* Constructor
	*/
	SandBox::SandBox(const std::string& path) : _path(path)
	{
		_initialize();
	}

	/**
	* Destructor
	*/
	SandBox::~SandBox()
	{
	}

	/**
	* Initialize SandBox duty
	*/
	void SandBox::_initialize()
	{
		// TODO
	}
}
