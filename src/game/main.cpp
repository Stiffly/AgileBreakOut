#include "PrecompiledHeader.h"
#include "Core/Engine.h"

int main(int argc, char* argv[])
{
	dd::Engine engine(argc, argv);
	LOG_INFO("------------ Engine initialized ------------");
	while (engine.Running())
		engine.Tick();

	return 0;
}