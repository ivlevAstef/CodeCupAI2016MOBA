#include "S_CommandStrategy.h"

using namespace AICup;

CommandStategy::CommandStategy(const CommandFabric& fabric) : fabric(fabric) {
}

#ifdef ENABLE_VISUALIZATOR
void AICup::CommandStategy::visualization(const Visualizator & visualizator) const {
}
#endif
