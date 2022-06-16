#include "uci_base.h"

using namespace std;

namespace uci
{
	void UciBase::getline()
	{
		// --- get line from stream (blocking) ---
		std::string line;

		if (is && std::getline(is, line) && !line.empty()) {

			if (line.back() == '\r') {
				line.pop_back();
			}

			Command cmd;
			cmd.parse(line);
			
			// --- append to command list ---
			if (cmd.isValid()) {
				commands_in.emplace_back(std::move(cmd));
			}
		}
	}
} // namespace uci
