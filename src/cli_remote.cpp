//
// Remote command line controller for UERANSIM UEs.
//
// Controls a set of UERANSIM UE processes, each with an undefined set of UEs.
// All UE processes are specified with IP and port in a file that must be
// provided as input to this binary.
//
// Functionality supported:
// * issue ServiceRequest to all UEs
//
// 
//

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include <lib/app/cli_base.hpp>
#include <lib/app/proc_table.hpp>
#include <utils/common.hpp>
#include <utils/constants.hpp>
#include <utils/io.hpp>
#include <utils/network.hpp>
#include <utils/options.hpp>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

static struct Options
{
    bool dumpNodes{};
    std::string clientFile{};
} g_options{};

static void ReadOptions(int argc, char **argv)
{
    opt::OptionsDescription desc{"UERANSIM",  cons::Tag, "Command Line Interface",
        cons::Owner, "remote-cli",  {"<client-file> [option...]", "--dump"},
        {},          true,      false};

    opt::OptionItem itemDump = {'d', "dump", "List all UE processes", std::nullopt};

    desc.items.push_back(itemDump);

    opt::OptionsResult opt{argc, argv, desc, false, nullptr};

    g_options.dumpNodes = opt.hasFlag(itemDump);

    if (opt.positionalCount() == 0)
    {
        opt.showError("Client file is expected");
        return;
    }
    if (opt.positionalCount() > 1)
    {
        opt.showError("Only one Client file is expected");
        return;
    }

    g_options.clientFile = opt.getPositional(0);

}

static int ReadClients(std::string clientFileName, std::vector<InetAddress> &clients)
{
    std::ifstream clientFile(clientFileName);
    std::string ip_address;
    int port;

    while (clientFile >> ip_address >> port){
        clients.push_back(InetAddress{ip_address, port});
    }

    return 0;
}


int main(int argc, char **argv)
{
    std::ifstream clientFile;
    std::vector<InetAddress> clients;
    app::CliServer server{};

    ReadOptions(argc, argv);

    // Read the list of nodes from file
    if (ReadClients(g_options.clientFile, clients))
    {
        throw std::runtime_error("Client file could not be opened!");
    }

    if (g_options.dumpNodes)
    {
        for (InetAddress client : clients){
            std::cout << inet_ntoa(((struct sockaddr_in*)client.getSockAddr())->sin_addr)
                        << ":" << client.getPort() << std::endl;
        }
    }

    // Send the command to all the clients
    for (InetAddress client : clients)
    {
        server.sendMessage(app::CliMessage::ServiceRequest(client));
        app::CliMessage msg = server.receiveMessage();
        if (msg.type == app::CliMessage::Type::RESULT)
        {
            std::cout << inet_ntoa(((struct sockaddr_in*)msg.clientAddr.getSockAddr())->sin_addr)
                        << ":" << msg.clientAddr.getPort() << "  " << msg.value << std::endl;
        }
    }

    return 0;
}
