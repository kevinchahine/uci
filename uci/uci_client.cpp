#include "uci_client.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>	

#include <boost/process.hpp>

#include <numeric>		// for accumulate

using namespace std;

namespace uci
{
	UciClient::UciClient(const boost::filesystem::path& engine_file_path)
	{
		launch(engine_file_path);
	}

	void UciClient::launch(const boost::filesystem::path& engine_file_path)
	{
		// --- 1.) Make sure engine application exists ---
		const boost::filesystem::path & path = engine_file_path;
		cout << "Engine path: " << path << endl;

		if (path.is_absolute()) {
			cout << "Path is absolute" << endl;
		}
		else {
			cout << "Path is relative" << endl;
			// app can be in:
			//	- current currectory
			//	- under system path
			if (boost::filesystem::exists(path)) {
				cout << "app exists" << endl;
			}
			else {
				// Check under system path
				// boost::filesystem::path::find
				// TODO: Do this
			}
		}

		//boost::filesystem::path fromPath = boost::process::search_path(path);

		//cout << "from path: " << fromPath << endl;

		if (boost::filesystem::exists(path) == false) {
			cout << "Engine is not found or does not exist" << endl;

			return;
		}

		// --- 2.) Open engine ---
		engine = boost::process::child{
			engine_file_path,
			boost::process::std_out > is,
			boost::process::std_in < os
		};

		//std::string currline;
		//
		//while (is && std::getline(is, currline) && !currline.empty()) {
		//	std::cerr << "\'" << currline << "\'" << std::endl;
		//}
		//
		//cout << "done" << endl;

		boost::filesystem::path filename = path.filename();
		if (filename.has_extension()) {
			filename = filename.leaf();
		}

		name = filename.string();	// TODO: strip filename extension if it exists
	}

	void UciClient::close()
	{
		// TODO: This might hang
		//engine.wait();
	}

	void UciClient::send_uci()
	{
		os << "uci" << endl;
	}

	void UciClient::send_debug(const Debug& debug)
	{
		os << "debug ";

		switch (debug)
		{
		case Debug::on:		os << "on";		break;
		case Debug::off:	os << "off";	break;
		}

		os << endl;
	}

	void UciClient::send_isready()
	{
		os << "isready" << endl;
	}

	void UciClient::send_ucinewgame()
	{
		os << "ucinewgame" << endl;
	}

	void UciClient::send_position()
	{
		os << "position startpos" << endl;
	}

	void UciClient::send_position(const string fen)
	{
		if (fen == "startpos" || fen.empty()) {
			os << "position startpos" << endl;
		}
		else {
			os << "position fen " << fen << endl;
		}
	}

	void UciClient::send_position(const std::string fen, const std::vector<std::string>& moveSequence)
	{
		if (fen == "startpos") {
			os << "position startpos";
		}
		else {
			os << "position fen " << fen;
		}

		if (moveSequence.size()) {
			os << " moves";

			for (const std::string& move : moveSequence) {
				os << ' ' << move;
			}
		}

		os << endl;
	}

	void UciClient::send_go(const go& go_params)
	{
		os << go_params << endl;
	}

	void UciClient::send_stop()
	{
		os << "stop" << endl;
	}

	void UciClient::send_ponderhit()
	{
		os << "ponderhit" << endl;
	}

	void UciClient::send_quit()
	{
		os << "quit" << endl;
	}

	// ----------------------------------- WAITS ---------------------------------

	const Command & UciClient::recv_until(const std::string& cmd_to_wait_for)
	{
		while (true) {
			// Reads next currline from input stream. 
			// Blocking call. Blocks until entire currline is read.
			// If currline is a valid command, it will be appended to back of `commands_in`
			// Invalid commands will be deleted and method will return
			this->parse_line(is);

			if (commands_in.size()) {
				const Command& cmd = commands_in.back();

				if (cmd.cmd() == cmd_to_wait_for) {
					break;
				}
			}
		}

		return commands_in.back();
	}

	const Command & UciClient::recv_until_id() 
	{
		return recv_until("id");
	}

	const Command & UciClient::recv_until_uciok() 
	{
		return recv_until("uciok");
	}

	const Command & UciClient::recv_until_readyok()
	{
		return recv_until("readyok");
	}

	const Command & UciClient::recv_until_bestmove()
	{
		return recv_until("bestmove");
	}

	const Command & UciClient::recv_until_copyprotection() 
	{
		return recv_until("copyprotection");
	}

	const Command & UciClient::recv_until_registration() 
	{
		return recv_until("registration");
	}

	const Command & UciClient::recv_until_info() 
	{
		return recv_until("info");
	}

	const Command & UciClient::recv_until_option()
	{
		return recv_until("option");
	}

	// ----------------------------------- CALLBACKS -----------------------------
	
	void UciClient::on_any_command(const Command& cmd)
	{
		const string& cmd_name = cmd.cmd();

		if (false) { /* placeholder */ }
		else if (cmd_name == "id") { this->on_id(cmd); }
		else if (cmd_name == "uciok") { this->on_uciok(cmd); }
		else if (cmd_name == "readyok") { this->on_readyok(cmd); }
		else if (cmd_name == "bestmove") { this->on_bestmove(cmd); }
		else if (cmd_name == "copyprotection") { this->on_copyprotection(cmd); }
		else if (cmd_name == "register") { this->on_register(cmd); }
		else if (cmd_name == "info") { this->on_info(cmd); }
		else if (cmd_name == "option") { this->on_option(cmd); }
		else {
			cout << "Error " << __FILE__ << " line " << __LINE__
				<< ": command is not recognized " << cmd << endl;
		}
	}

	void UciClient::on_id(const Command& cmd) {}
	void UciClient::on_uciok(const Command& cmd) {}
	void UciClient::on_readyok(const Command& cmd) {}
	void UciClient::on_bestmove(const Command& cmd) {}
	void UciClient::on_copyprotection(const Command& cmd) {}
	void UciClient::on_register(const Command& cmd) {}
	void UciClient::on_info(const Command& cmd) {}

	void UciClient::on_option(const Command& cmd)
	{
		// TODO: Search options to see if this option has already been specified
		// If so overwrite it.
		options.emplace_back(cmd);
	}

	// ------------------ ASYNC -------------------------------------------

	void UciClient::init()
	{
		//std::function<void(const boost::system::error_code&, std::size_t)> callback =
		//	[&](const boost::system::error_code& ec, std::size_t size) {
		//	cout << "It worked: " << size << ' ';
		//
		//	std::istream is(&sbuf);
		//
		//	string line;
		//	getline(is, line);
		//
		//	cout << line << endl;
		//
		//	//boost::asio::async_read(ap_in, boost::asio::buffer(buf), callback);
		//	boost::asio::async_read_until(is, sbuf, '\n', callback);
		//
		//	//cout << "Type UCI command to send to engine:";
		//	//string line;
		//	//getline(cin, line);
		//	//
		//	//pipe_out << line << endl;
		//};
		//
		////boost::asio::async_read(ap_in, boost::asio::buffer(buf), callback);
		//boost::asio::async_read_until(ap_in, sbuf, '\n', callback);
		
		// ------------------ DISPATCH CALLBACKS ------------------------------
		//context.dispatch([this]() { this->on_any_command(); });
	}

	void UciClient::update()
	{
		context.poll();
	}
} // namespace uci
